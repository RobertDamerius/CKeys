#include <Recorder.hpp>
#include <MainWindow.hpp>
#include <MIDIFile.hpp>


Recorder::Recorder(){
    track.channel = 0;
    track.name.clear();
    track.instrumentType = 0;
    track.colorWhiteKey = glm::u8vec3(180,0,0);
    track.colorBlackKey = glm::u8vec3(100,0,0);
    midiIn = nullptr;
    runningStatus = 0x00;
    isRecording = false;
    latestTimePointer = 0.0;
}

Recorder::~Recorder(){
    StopRecording();
}

double Recorder::GetTimePointer(void){
    if(isRecording){
        auto timeNow = std::chrono::steady_clock::now();
        return (latestTimePointer = 1e-9 * double(std::chrono::duration_cast<std::chrono::nanoseconds>(timeNow - timeOfStart).count()));
    }
    return latestTimePointer;
}

bool Recorder::StartRecording(void){
    // Make sure that recording is stopped
    StopRecording();

    // Reset track
    rawRecordedData.clear();
    mtxTrack.lock();
    for(int i = 0; i < 88; i++){
        track.lanes[i].clear();
    }
    mtxTrack.unlock();
    runningStatus = 0x00;
    latestTimePointer = 0.0;

    // Create RT-MIDI object
    try{
        midiIn = new RtMidiIn();
    }
    catch(RtMidiError &error){
        midiIn = nullptr;
        return false;
    }

    // Use port 0 as default port and check number of available ports
    unsigned int port = 0;
    unsigned int nPorts = midiIn->getPortCount();
    if(port >= nPorts){
        delete midiIn;
        midiIn = nullptr;
        return false;
    }

    // Don't ignore sysex, timing, or active sensing messages.
    midiIn->ignoreTypes(false, false, false);

    // Open MIDI port
    try{
        midiIn->openPort(port);
    }
    catch(RtMidiError &error){
        delete midiIn;
        midiIn = nullptr;
        return false;
    }
    MainWindow::canvas.renderer.SetPostProcessingColorScale(glm::vec3(1.0f,0.74f,0.74f));

    // Get and ignore all current messages in the buffer
    std::vector<unsigned char> msg;
    do {
        (void)midiIn->getMessage(&msg);
    } while(msg.size());

    // Set callback and return success
    void *userData = (void*)this;
    midiIn->setCallback(&(Recorder::CallbackMidiIn), userData);
    return true;
}

void Recorder::StopRecording(void){
    if(midiIn){
        midiIn->cancelCallback();
        midiIn->closePort();
        delete midiIn;
        midiIn = nullptr;
        MainWindow::canvas.renderer.SetPostProcessingColorScale(glm::vec3(1.0f));
    }
    isRecording = false;
}

void Recorder::ReceiveMIDI(double timestamp, std::vector<unsigned char>& message){
    // Start actual recording when first message is received
    double absoluteTime = 0.0;
    if(!isRecording){
        isRecording = true;
        timeOfStart = std::chrono::steady_clock::now();
    }
    else{
        auto timeNow = std::chrono::steady_clock::now();
        absoluteTime = 1e-9 * double(std::chrono::duration_cast<std::chrono::nanoseconds>(timeNow - timeOfStart).count());
    }

    // Check for running status
    if(!message.size()) return;
    int index = 0;
    if(0x80 & message[0]){
        runningStatus = message[0];
        index++;
    }

    // Process note on/off events
    if((2 == ((int)message.size() - index)) && ((0x80 == (runningStatus & 0xF0)) || (0x90 == (runningStatus & 0xF0)))){
        uint8_t key = message[index];
        uint8_t vel = message[index + 1] & 0x7F;
        bool noteOff = (0x80 == (runningStatus & 0xF0)) || ((0x90 == (runningStatus & 0xF0)) && !vel);

        // only keys between A0 and C8
        if((key >= 21) && (key <= 108)){
            key -= 21;
            mtxTrack.lock();
            if(!noteOff){
                track.lanes[key].push_back(NoteBlock(double(vel) / 127.0, absoluteTime));
            }
            else if(track.lanes[key].size()){
                track.lanes[key].back().off = absoluteTime;
            }
            mtxTrack.unlock();
        }
    }

    // Save raw data
    rawRecordedData.push_back({timestamp, std::move(message)});
}

bool Recorder::Save(void){
    const std::lock_guard<std::mutex> lock(mtxTrack);

    // Directory of application
    char* buffer = new char[65536];
    #ifdef __WIN32__
    DWORD len = GetModuleFileName(NULL, (LPSTR)(&buffer[0]), (DWORD)65536);
    #else
    ssize_t len = readlink("/proc/self/exe", &buffer[0], 65536);
    #endif
    std::string str(buffer, len);
    auto found = str.find_last_of("/\\");
    std::string path = str.substr(0, found + 1);
    delete[] buffer;

    // Generate filename
    auto systemClock = std::chrono::system_clock::now();
    std::time_t systemTime = std::chrono::system_clock::to_time_t(systemClock);
    std::tm* gmTime = std::gmtime(&systemTime);
    char name[64];
    sprintf(name,"Recording%d%02d%02d%02d%02d%02d.mid", gmTime->tm_year + 1900, gmTime->tm_mon + 1, gmTime->tm_mday, gmTime->tm_hour, gmTime->tm_min, gmTime->tm_sec);
    std::string filename = path + std::string(name);

    // MIDI Header
    MIDIFile midi;
    midi.header.format = 1;        // multi-track
    midi.header.numTracks = 2;     // 2 tracks
    midi.header.division = 0x7800; // ticks per quarter note: 30720
    double time2Ticks = (double)(midi.header.division) / 0.5; // 0.5 because of 120 BPM

    // MIDI Track 1 (info track)
    midi.tracks.push_back(MIDIChunkTrack());
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xFF;    midi.tracks.back().events.back().data = {0x03, 0x00};                          // Name: ""
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xFF;    midi.tracks.back().events.back().data = {0x51, 0x03, 0x07, 0xA1, 0x20};        // Tempo: 120 BPM
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xFF;    midi.tracks.back().events.back().data = {0x58, 0x04, 0x04, 0x02, 0x07, 0xA1};  // Time signatur: 4/4
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xFF;    midi.tracks.back().events.back().data = {0x2F, 0x00};                          // End of track

    // MIDI Track 2 (instrument track)
    midi.tracks.push_back(MIDIChunkTrack());
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xFF;    midi.tracks.back().events.back().data = {0x03, 0x00};                          // Name: ""
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xC0;    midi.tracks.back().events.back().data = {0x00};                                // Program Change: Channel 0 -> Piano
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xC1;    midi.tracks.back().events.back().data = {0x00};                                // Program Change: Channel 1 -> Piano
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xC2;    midi.tracks.back().events.back().data = {0x00};                                // Program Change: Channel 2 -> Piano
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xC3;    midi.tracks.back().events.back().data = {0x00};                                // Program Change: Channel 3 -> Piano
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xC4;    midi.tracks.back().events.back().data = {0x00};                                // Program Change: Channel 4 -> Piano
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xC5;    midi.tracks.back().events.back().data = {0x00};                                // Program Change: Channel 5 -> Piano
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xC6;    midi.tracks.back().events.back().data = {0x00};                                // Program Change: Channel 6 -> Piano
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xC7;    midi.tracks.back().events.back().data = {0x00};                                // Program Change: Channel 7 -> Piano
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xC8;    midi.tracks.back().events.back().data = {0x00};                                // Program Change: Channel 8 -> Piano
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xC9;    midi.tracks.back().events.back().data = {0x00};                                // Program Change: Channel 9 -> Piano
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xCA;    midi.tracks.back().events.back().data = {0x00};                                // Program Change: Channel 10 -> Piano
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xCB;    midi.tracks.back().events.back().data = {0x00};                                // Program Change: Channel 11 -> Piano
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xCC;    midi.tracks.back().events.back().data = {0x00};                                // Program Change: Channel 12 -> Piano
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xCD;    midi.tracks.back().events.back().data = {0x00};                                // Program Change: Channel 13 -> Piano
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xCE;    midi.tracks.back().events.back().data = {0x00};                                // Program Change: Channel 14 -> Piano
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xCF;    midi.tracks.back().events.back().data = {0x00};                                // Program Change: Channel 15 -> Piano
    double tickError = 0.0;
    for(auto&& msg : rawRecordedData){
        if(msg.second.size()){
            double tick = tickError + time2Ticks * msg.first;
            midi.tracks.back().events.push_back(MIDIEvent());
            midi.tracks.back().events.back().deltaTime = (uint32_t)tick;
            tickError = (tick - (double)midi.tracks.back().events.back().deltaTime);
            midi.tracks.back().events.back().status = msg.second[0];
            for(int i = 1; i < (int)msg.second.size(); i++){
                midi.tracks.back().events.back().data.push_back(msg.second[i]);
            }
        }
    }
    midi.tracks.back().events.push_back(MIDIEvent());    midi.tracks.back().events.back().deltaTime = 0;    midi.tracks.back().events.back().status = 0xFF;    midi.tracks.back().events.back().data = {0x2F, 0x00};                          // End of track

    // Write MIDI file
    return midi.Write(filename);
}

