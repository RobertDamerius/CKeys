#include <MIDIChunkTrack.hpp>
#include <MIDIFile.hpp>


bool MIDIChunkTrack::Decode(const uint8_t* chunkData, const uint32_t length){
    this->events.clear();
    std::vector<MIDIEvent> events;
    uint32_t index = 0;
    uint8_t previousStatus = 0;
    while(index < length){
        // Decode delta-time
        uint32_t deltaTime = 0;
        for(int i = 0; (i < 4) && (index < length); i++){
            deltaTime <<= 7;
            deltaTime |= (chunkData[index++] & 0x7F);
            if(!(0x80 & chunkData[index - 1])){
                break;
            }
        }

        // Decode events (type of event is indicated by first status byte)
        if((index + 1) > length) return false;
        uint8_t status = chunkData[index++];
        if(!(status & 0x80)){
            // Running status (use previous status)
            --index;
            status = previousStatus;
        }
        previousStatus = status;

        // Channel Voice Messages
        if(0x80 == (status & 0xF0)){ // Note Off Event
            if((index + 2) > length) return false;
            events.push_back(MIDIEvent(deltaTime, status, &chunkData[index], 2));
            index += 2;
        }
        else if(0x90 == (status & 0xF0)){ // Note On Event
            if((index + 2) > length) return false;
            events.push_back(MIDIEvent(deltaTime, status, &chunkData[index], 2));
            index += 2;
        }
        else if(0xA0 == (status & 0xF0)){ // Polyphonic Key Pressure (Aftertouch)
            if((index + 2) > length) return false;
            events.push_back(MIDIEvent(deltaTime, status, &chunkData[index], 2));
            index += 2;
        }
        else if(0xB0 == (status & 0xF0)){ // Control Change or Channel Mode Message
            if((index + 2) > length) return false;
            events.push_back(MIDIEvent(deltaTime, status, &chunkData[index], 2));
            index += 2;
        }
        else if(0xC0 == (status & 0xF0)){ // Program Change
            if((index + 1) > length) return false;
            events.push_back(MIDIEvent(deltaTime, status, &chunkData[index], 1));
            index += 1;
        }
        else if(0xD0 == (status & 0xF0)){ // Channel Pressure (After-touch)
            if((index + 1) > length) return false;
            events.push_back(MIDIEvent(deltaTime, status, &chunkData[index], 1));
            index += 1;
        }
        else if(0xE0 == (status & 0xF0)){ // Pitch Wheel Change
            if((index + 2) > length) return false;
            events.push_back(MIDIEvent(deltaTime, status, &chunkData[index], 2));
            index += 2;
        }
        // System Common Messages
        else if(0xF0 == status){ // System Exclusive, 0xF7 indicates end of exlusive
            MIDIEvent evt(deltaTime, status);
            while(index < length){
                evt.data.push_back(chunkData[index++]);
                if(0xF7 == evt.data.back()) break;
            }
            events.push_back(std::move(evt));
        }
        else if(0xF1 == status){ // Undefined
        }
        else if(0xF2 == status){ // Song Position Pointer
            if((index + 2) > length) return false;
            events.push_back(MIDIEvent(deltaTime, status, &chunkData[index], 2));
            index += 2;
        }
        else if(0xF3 == status){ // Song Select
            if((index + 1) > length) return false;
            events.push_back(MIDIEvent(deltaTime, status, &chunkData[index], 1));
            index += 1;
        }
        else if(0xF4 == status){ // Undefined
        }
        else if(0xF5 == status){ // Undefined
        }
        else if(0xF6 == status){ // Tune Request
            if((index + 1) > length) return false;
            events.push_back(MIDIEvent(deltaTime, status));
        }
        else if(0xF7 == status){ // End Of Exclusive
            // This is part of data in the 0xF0 event
        }
        // System Real-Time Messages
        else if(0xF8 == status){ // Timing Clock
            if((index + 1) > length) return false;
            events.push_back(MIDIEvent(deltaTime, status));
        }
        else if(0xF9 == status){ // Undefined
        }
        else if(0xFA == status){ // Start
            if((index + 1) > length) return false;
            events.push_back(MIDIEvent(deltaTime, status));
        }
        else if(0xFB == status){ // Continue
            if((index + 1) > length) return false;
            events.push_back(MIDIEvent(deltaTime, status));
        }
        else if(0xFC == status){ // Stop
            if((index + 1) > length) return false;
            events.push_back(MIDIEvent(deltaTime, status));
        }
        else if(0xFD == status){ // Undefined
        }
        else if(0xFE == status){ // Active Sensing
            if((index + 1) > length) return false;
            events.push_back(MIDIEvent(deltaTime, status));
        }
        // Meta Events
        else if(0xFF == status){ // Meta events
            MIDIEvent evt(deltaTime, status);
            if((index + 1) > length) return false;
            evt.data.push_back(chunkData[index++]);
            uint32_t metaEventLength = 0;
            for(int i = 0; (i < 4) && (index < length); i++){
                evt.data.push_back(chunkData[index++]);
                metaEventLength <<= 7;
                metaEventLength |= (evt.data.back() & 0x7F);
                if(!(0x80 & evt.data.back())){
                    break;
                }
            }
            if((index + metaEventLength) > length) return false;
            evt.data.insert(evt.data.end(), &chunkData[index], &chunkData[index + metaEventLength]);
            index += metaEventLength;
            events.push_back(std::move(evt));
        }
    }
    this->events.swap(events);
    return true;
}

bool MIDIChunkTrack::Encode(std::vector<uint8_t>& bytes){
    for(auto&& event : events){
        uint32_t dt = 0x0FFFFFFF & event.deltaTime;
        uint8_t b3 = (0x0000007F & (dt >> 21));
        uint8_t b2 = (0x0000007F & (dt >> 14));
        uint8_t b1 = (0x0000007F & (dt >> 7));
        uint8_t b0 = (0x0000007F & dt);
        if(dt > 0x001FFFFF){
            bytes.push_back(0x80 | b3);
        }
        if(dt > 0x00003FFF){
            bytes.push_back(0x80 | b2);
        }
        if(dt > 0x0000007F){
            bytes.push_back(0x80 | b1);
        }
        bytes.push_back(b0);
        bytes.push_back(event.status);
        for(unsigned i = 0; i < (unsigned)event.data.size(); i++){
            bytes.push_back(event.data[i]);
        }
    }
    return true;
}

std::string MIDIChunkTrack::GetSequenceTrackName(void){
    std::string result;
    for(auto&& event : events){
        if((0xFF == event.status) && (event.data.size() > 1) && (0x03 == event.data[0])){
            uint32_t bytesRead;
            uint32_t len = MIDIFile::ReadVariableLength(bytesRead, &event.data[1], (uint32_t)event.data.size() - 1);
            if(event.data.size() != (1 + bytesRead + len)){
                break;
            }
            for(uint32_t k = 1 + bytesRead; k < event.data.size(); k++){
                result.push_back((char)event.data[k]);
            }
            break;
        }
    }
    return result;
}

