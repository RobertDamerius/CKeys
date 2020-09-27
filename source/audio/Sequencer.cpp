#include <Sequencer.hpp>
#include <MIDIFile.hpp>
#include <AudioEngine.hpp>


Sequencer::Sequencer(){
    this->ticksPerQuarter = 1;
    this->maxNumSamples = 0;
    this->currentSample = 0;
}

bool Sequencer::ReadMIDIFile(std::string filename){
    // Read MIDI file
    MIDIFile midi;
    this->tracks.clear();
    this->tempoChanges.clear();
    this->ticksPerQuarter = 1;
    if(!midi.Read(filename)){
        LogError("Could not read MIDI file \"%s\"!\n",filename.c_str());
        return false;
    }

    // Check for supported MIDI and timecode format
    if((0 != midi.header.format) && (1 != midi.header.format)){
        LogError("Format %d of MIDI file \"%s\" is not supported!\n",midi.header.format,filename.c_str());
        return false;
    }
    if(0x8000 & midi.header.division){
        LogError("SMPTE timecode format of MIDI file \"%s\" is not supported!\n",midi.header.format,filename.c_str());
        return false;
    }
    if(!midi.tracks.size()){
        LogWarning("MIDI file \"%s\" contains no tracks!\n",filename.c_str());
        return false;
    }

    // First track always contains song name
    name = midi.tracks[0].GetSequenceTrackName();

    // Convert all MIDI tracks to sequence tracks (ignore empty tracks)
    uint32_t n = (0 == midi.header.format) ? 0 : 1;
    std::map<uint8_t, std::vector<MIDIEvent>> channelPedalChanges;
    for(; n < (uint32_t)midi.tracks.size(); n++){
        std::string trackName;
        if(n){
            trackName = midi.tracks[n].GetSequenceTrackName();
        }
        std::map<uint8_t, SequenceTrack> channelTracks;
        for(auto&& event : midi.tracks[n].events){
            if((0xC0 == (0xF0 & event.status)) && (1 == event.data.size())){
                uint8_t ch = event.status & 0x0F;
                const auto res = channelTracks.insert({ch, SequenceTrack(ch, trackName)});
                res.first->second.instrumentType = event.data[0];
            }
            else if((0x80 == (0xF0 & event.status)) || (0x90 == (0xF0 & event.status))){
                uint8_t ch = event.status & 0x0F;
                const auto res = channelTracks.insert({ch, SequenceTrack(ch, trackName)});
                res.first->second.midiEvents.push_back(event);
            }
            else if((0xB0 == (0xF0 & event.status)) && (2 == event.data.size()) && (0x40 == event.data[0])){
                uint8_t ch = event.status & 0x0F;
                const auto res = channelPedalChanges.insert({ch, std::vector<MIDIEvent>()});
                res.first->second.push_back(event);
            }
        }
        for(auto&& ct : channelTracks){
            if(ct.second.midiEvents.size()){
                this->tracks.push_back(ct.second);
            }
        }
    }
    for(auto&& track : this->tracks){
        auto got = channelPedalChanges.find(track.channel);
        if(got != channelPedalChanges.end()){
            for(auto&& evt : got->second){
                track.sustainPedalChanges.push_back({evt.absoluteTicks, (evt.data[1] >= 64)});
            }
        }
    }

    // Set default color values
    for(uint32_t u = 0; u < (uint32_t)this->tracks.size(); u++){
        this->tracks[u].SetColor(u);
    }

    // Find all tempo changes (tempo = seconds per quarter note)
    std::map<uint64_t, double> tempoChanges;
    for(auto&& track : midi.tracks){
        for(auto&& event : track.events){
            if((0xFF == event.status) && (5 == event.data.size()) && (0x51 == event.data[0]) && (0x03 == event.data[1])){
                uint32_t usPerQuarter = (uint32_t(event.data[2]) << 16) | (uint32_t(event.data[3]) << 8) | uint32_t(event.data[4]);
                tempoChanges.insert({event.absoluteTicks, 1e-6 * double(usPerQuarter)});
            }
        }
    }
    auto got = tempoChanges.find(0);
    if(got == tempoChanges.end()){
        tempoChanges.insert({0, 0.5}); // 120 BPM as default tempo
    }
    for(auto it = tempoChanges.begin(); it != tempoChanges.end(); ++it){
        this->tempoChanges.push_back(*it);
    }
    this->ticksPerQuarter = midi.header.division;
    return true;
}

void Sequencer::Generate(double tempoScale){
    double timeScale = 1.0 / std::clamp(tempoScale, SEQUENCER_TEMPO_SCALE_MIN, SEQUENCER_TEMPO_SCALE_MAX);
    double timeMax = 0.0;
    for(auto&& track : tracks){
        // Remove all note blocks
        for(int i = 0; i < 88; i++){
            track.lanes[i].clear();
        }

        // Process all midi events of this track
        for(auto&& me : track.midiEvents){
            if((0x90 == (0XF0 & me.status)) && (2 == me.data.size())){ // Note On
                uint8_t key = me.data[0];
                uint8_t vel = me.data[1] & 0x7F;
                if((key < 21) || (key > 108)) // only keys between A0 and C8
                    continue;
                key -= 21;
                if(vel){ // Note On
                    double timeOn = timeScale * GetTimestamp(me.absoluteTicks);
                    timeMax = std::max(timeMax, timeOn);
                    track.lanes[key].push_back(NoteBlock(double(vel) / 127.0, timeOn));
                }
                else{ // Note Off: Note On events with velocity zero are note off events
                    double timeOff = timeScale * GetTimestamp(me.absoluteTicks);
                    timeMax = std::max(timeMax, timeOff);
                    if(track.lanes[key].size()){
                        track.lanes[key].back().sustainOff = track.lanes[key].back().off = std::min(track.lanes[key].back().off, timeOff);
                        if(GetPedalState(me.absoluteTicks, track.sustainPedalChanges)){
                            track.lanes[key].back().sustainOff = timeScale * GetTimestampOfNextPedalOff(me.absoluteTicks, track.sustainPedalChanges);
                        }
                    }
                    // Remove possible missing or too large note off events from previous notes
                    int32_t N = (int32_t)track.lanes[key].size();
                    for(int32_t n = N - 1; n >= 0; --n){
                        track.lanes[key][n].off = std::min(track.lanes[key][n].off, timeOff);
                    }
                }
            }
            else if((0x80 == (0xF0 & me.status)) && (2 == me.data.size())){ // Note Off
                uint8_t key = me.data[0];
                if((key < 21) || (key > 108)) // only keys between A0 and C8
                    continue;
                key -= 21;
                double timeOff = timeScale * GetTimestamp(me.absoluteTicks);
                timeMax = std::max(timeMax, timeOff);
                if(track.lanes[key].size()){
                    track.lanes[key].back().sustainOff = track.lanes[key].back().off = std::min(track.lanes[key].back().off, timeOff);
                    if(GetPedalState(me.absoluteTicks, track.sustainPedalChanges)){
                        track.lanes[key].back().sustainOff = timeScale * GetTimestampOfNextPedalOff(me.absoluteTicks, track.sustainPedalChanges);
                    }
                }
                // Remove possible missing or too large note off events from previous notes
                int32_t N = (int32_t)track.lanes[key].size();
                for(int32_t n = N - 1; n >= 0; --n){
                    track.lanes[key][n].off = std::min(track.lanes[key][n].off, timeOff);
                }
            }
        }
    }

    // Replace INF times with maximum time (make inf times obviously greater (5 sec) than maximum time of whole performance)
    // This is just a fallback solution. Usually all notes have on and off events. But we want to prevent the GUI from trying to render INF in case of corrupted MIDI files.
    timeMax += 5.0;
    maxNumSamples = 0;
    currentSample = 0;
    for(auto&& track : tracks){
        for(int i = 0; i < 88; i++){
            for(auto&& noteBlock : track.lanes[i]){
                noteBlock.off = std::min(noteBlock.off, timeMax);
                noteBlock.sustainOff = std::min(noteBlock.sustainOff, timeMax);
            }
        }

        // Let the audio engine generate audio samples for this track
        AudioEngine::RenderSound(track);
        maxNumSamples = std::max(maxNumSamples, (uint32_t)track.samples.size());
    }
}

double Sequencer::GetTimestamp(uint64_t tick){
    if(!tick) return 0.0;
    double result = 0.0;
    uint32_t idxLatestTempoChange = 0;
    for(uint32_t k = 1; k < (uint32_t)tempoChanges.size(); k++){
        if(tempoChanges[k].first < tick){
            result += (double(tempoChanges[k].first - tempoChanges[idxLatestTempoChange].first) / double(this->ticksPerQuarter)) * tempoChanges[idxLatestTempoChange].second;
            idxLatestTempoChange = k;
        }
    }
    result += (double(tick - tempoChanges[idxLatestTempoChange].first) / double(this->ticksPerQuarter)) * tempoChanges[idxLatestTempoChange].second;
    return result;
}

bool Sequencer::GetPedalState(uint64_t tick, std::vector<std::pair<uint64_t, bool>>& sustainPedalChanges){
    bool result = false;
    for(auto&& a : sustainPedalChanges){
        if(a.first > tick)
            break;
        result = a.second;
    }
    return result;
}

double Sequencer::GetTimestampOfNextPedalOff(uint64_t tick, std::vector<std::pair<uint64_t, bool>>& sustainPedalChanges){
    for(auto&& a : sustainPedalChanges){
        if((a.first > tick) && !a.second){
            return GetTimestamp(a.first);
        }
    }
    return std::numeric_limits<double>::infinity();
}

