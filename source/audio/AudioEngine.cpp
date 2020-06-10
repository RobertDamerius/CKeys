#include <AudioEngine.hpp>
#include <MainWindow.hpp>
#include <Sequencer.hpp>


// Github allows file sizes to be at most 100 MB, so sound font is splited into two parts
// RESOURCE_EXTLD(source_thirdparty_TinySoundFont_soundfont_bin);
RESOURCE_EXTLD(source_thirdparty_TinySoundFont_soundfont_part1_bin);
RESOURCE_EXTLD(source_thirdparty_TinySoundFont_soundfont_part2_bin);


bool AudioEngine::initialized = false;
tsf* AudioEngine::soundFont = nullptr;
PaStream* AudioEngine::audioStream = nullptr;
std::chrono::time_point<std::chrono::steady_clock> AudioEngine::timeOfStart;
double AudioEngine::outputLatency = 0.0;
double AudioEngine::timePointer = 0.0;
double AudioEngine::timePointerOfStart = 0.0;


bool AudioEngine::Initialize(void){
    // Make sure that the engine is terminated
    Terminate();

    // Load sound font and initialize TinySoundFont object
    // Github allows file sizes to be at most 100 MB, so sound font is splited into two parts
    // size_t len = RESOURCE_LDLEN(source_thirdparty_TinySoundFont_soundfont_bin);
    // const unsigned char* data = RESOURCE_LDVAR(source_thirdparty_TinySoundFont_soundfont_bin);
    // soundFont = tsf_load_memory(data, (int)len);
    size_t len1 = RESOURCE_LDLEN(source_thirdparty_TinySoundFont_soundfont_part1_bin);
    const unsigned char* data1 = RESOURCE_LDVAR(source_thirdparty_TinySoundFont_soundfont_part1_bin);
    size_t len2 = RESOURCE_LDLEN(source_thirdparty_TinySoundFont_soundfont_part2_bin);
    const unsigned char* data2 = RESOURCE_LDVAR(source_thirdparty_TinySoundFont_soundfont_part2_bin);
    std::vector<unsigned char> data(&data1[0], &data1[0] + len1);
    data.resize(len1 + len2);
    std::copy(&data2[0], &data2[0] + len2, data.begin() + len1);
    soundFont = tsf_load_memory(&data[0], (int)data.size());
    if(!soundFont){
        Terminate();
        return false;
    }
    tsf_set_output(soundFont, TSF_STEREO_INTERLEAVED, AUDIO_ENGINE_SAMPLE_RATE);

    // Setup port audio
    PaError err = Pa_Initialize();
    if(err != paNoError){
        LogError("Coult not initialize PortAudio!\n");
        Terminate();
        return false;
    }
    int numInputChannels = 0;
    int numOutputChannels = 2;
    void *userData = nullptr;
    err = Pa_OpenDefaultStream(&audioStream, numInputChannels, numOutputChannels, paFloat32, AUDIO_ENGINE_SAMPLE_RATE, AUDIO_ENGINE_SAMPLE_BUFFER_SIZE, AudioEngine::CallbackAudioStream, userData);
    if(err != paNoError){
        LogError("Could not open default audio stream!\n");
        Terminate();
        return false;
    }
    timeOfStart = std::chrono::steady_clock::now();
    outputLatency = 0.0;
    timePointer = 0.0;
    timePointerOfStart = 0.0;
    return (initialized = true);
}

void AudioEngine::Terminate(void){
    if(audioStream){
        (void) Pa_StopStream(audioStream);
        (void) Pa_CloseStream(audioStream);
        audioStream = nullptr;
    }
    timeOfStart = std::chrono::steady_clock::now();
    outputLatency = 0.0;
    timePointer = 0.0;
    timePointerOfStart = 0.0;
    Pa_Terminate();
    if(soundFont){
        tsf_close(soundFont);
        soundFont = nullptr;
    }
    initialized = false;
}

void AudioEngine::RenderSound(SequenceTrack& track){
    // Remove current samples
    track.samples.clear();
    if(!initialized)
        return;

    // Get maximum number of samples required to render the complete track
    double maxTime = -1.0;
    for(int i = 0; i < 88; i++){
        for(auto&& note : track.lanes[i]){
            maxTime = std::max(maxTime, note.sustainOff);
        }
    }
    if(maxTime <= 0.0)
        return;

    const double sampleRate = (double)AUDIO_ENGINE_SAMPLE_RATE;
    maxTime = std::clamp(maxTime + AUDIO_ENGINE_RELEASE_TIME_NOTE_OFF, 0.0, std::min(4294967295.0 / sampleRate, 21600.0)); // 6 hours at 22050 stereo are about 4 GB RAM !
    uint32_t maxNumSamples = (uint32_t)(maxTime * sampleRate);

    // Check if instrument of track is supported by the sound font
    int instrument = (int)track.instrumentType;
    if(instrument >= tsf_get_presetcount(soundFont)){
        return;
    }

    // Render audio samples for all note blocks
    tsf_reset(soundFont);
    int channel = (int)track.channel;
    tsf_channel_set_presetnumber(soundFont, channel, instrument, (AUDIO_ENGINE_MIDI_CHANNEL_DRUMS == channel) ? 1 : 0);
    std::vector<float> buffer(maxNumSamples * 2, 0.0f);
    for(int key = 0; key < 88; key++){
        for(auto&& note : track.lanes[key]){
            uint32_t idxOn = (uint32_t)(note.on * sampleRate);
            uint32_t idxOff = (uint32_t)(note.sustainOff * sampleRate);
            uint32_t idxReleased = (uint32_t)((note.sustainOff + AUDIO_ENGINE_RELEASE_TIME_NOTE_OFF) * sampleRate);
            tsf_channel_note_on(soundFont, channel, key + 21, (float)note.velocity);
            tsf_render_float(soundFont, &buffer[2*idxOn], idxOff - idxOn, 1);
            tsf_channel_note_off(soundFont, channel, key + 21);
            tsf_render_float(soundFont, &buffer[2*idxOff], idxReleased - idxOff, 1);
        }
    }
    track.samples.swap(buffer);
}

bool AudioEngine::StartStream(void){
    // Error if not initialized or sequencer has no samples
    if(!initialized) return false;
    if(!MainWindow::canvas.scene.performance.sequencer.maxNumSamples) return false;

    // Remember time pointer of start and get output latency
    timePointerOfStart = timePointer;
    const PaStreamInfo* info = Pa_GetStreamInfo(audioStream);
    if(info){
        outputLatency = info->outputLatency;
    }

    // Set index from where to start playing
    MainWindow::canvas.scene.performance.sequencer.currentSample = timePointerOfStart * (double)(2 * AUDIO_ENGINE_SAMPLE_RATE);
    if(MainWindow::canvas.scene.performance.sequencer.currentSample >= MainWindow::canvas.scene.performance.sequencer.maxNumSamples){
        MainWindow::canvas.scene.performance.sequencer.currentSample = !MainWindow::canvas.scene.performance.sequencer.maxNumSamples ? 0 : (MainWindow::canvas.scene.performance.sequencer.maxNumSamples - 1);
    }

    // Start streaming and remember system time
    bool result = (paNoError == Pa_StartStream(audioStream));
    timeOfStart = std::chrono::steady_clock::now();
    return result;
}

bool AudioEngine::StopStream(void){
    // Error if not initialized
    if(!initialized) return false;

    // Time difference from start to now
    if(StreamIsPlaying()){
        auto timeNow = std::chrono::steady_clock::now();
        double deltaTimeToStart = std::max(0.0, 1e-9 * double(std::chrono::duration_cast<std::chrono::nanoseconds>(timeNow - timeOfStart).count()) - outputLatency);
        timePointer = timePointerOfStart + deltaTimeToStart;
    }

    // Stop the stream (function waits until the stream is stopped completely)
    return (paNoError == Pa_StopStream(audioStream));
}

bool AudioEngine::StreamIsPlaying(void){
    if(!initialized) return false;
    return (1 == Pa_IsStreamActive(audioStream));
}

double AudioEngine::GetTimePointer(void){
    // Error if not initialized
    if(!initialized) return 0.0;

    // If stream is not active return current time pointer
    if(!Pa_IsStreamActive(audioStream)) return timePointer;

    // Time difference from start to now
    auto timeNow = std::chrono::steady_clock::now();
    double deltaTimeToStart = std::max(0.0, 1e-9 * double(std::chrono::duration_cast<std::chrono::nanoseconds>(timeNow - timeOfStart).count()) - outputLatency);

    // Update time pointer and return
    timePointer = timePointerOfStart + deltaTimeToStart;
    return timePointer;
}

void AudioEngine::SetTimePointer(double timePointer){
    if(StreamIsPlaying()) return;
    double maxTimePointer = (double)(MainWindow::canvas.scene.performance.sequencer.maxNumSamples / 2) / (double)(AUDIO_ENGINE_SAMPLE_RATE);
    AudioEngine::timePointer = std::clamp(timePointer, 0.0, maxTimePointer);
}

int AudioEngine::CallbackAudioStream(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData){
    uint32_t num = 2 * (uint32_t)frameCount;
    float *out = (float*)output;
    for(uint32_t k = 0, idx = MainWindow::canvas.scene.performance.sequencer.currentSample; k < num; k++, idx++){
        out[k] = 0.0f;
        for(auto&& track : MainWindow::canvas.scene.performance.sequencer.tracks){
            if(track.samples.size() && (idx < track.samples.size())){
                out[k] += track.samples[idx];
            }
        }
    }

    // Check if stream is completed
    MainWindow::canvas.scene.performance.sequencer.currentSample += num;
    if(MainWindow::canvas.scene.performance.sequencer.currentSample >= MainWindow::canvas.scene.performance.sequencer.maxNumSamples){
        MainWindow::canvas.scene.performance.sequencer.currentSample = !MainWindow::canvas.scene.performance.sequencer.maxNumSamples ? 0 : (MainWindow::canvas.scene.performance.sequencer.maxNumSamples - 1);
        return paComplete;
    }
    (void)input;
    (void)timeInfo;
    (void)statusFlags;
    (void)userData;
    return paContinue;
}

