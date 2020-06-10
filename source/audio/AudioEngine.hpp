#pragma once


#define AUDIO_ENGINE_SAMPLE_RATE             (22050) ///< Number of samples per second.
#define AUDIO_ENGINE_SAMPLE_BUFFER_SIZE      (256)   ///< Number of samples for audio buffer.
#define AUDIO_ENGINE_RELEASE_TIME_NOTE_OFF   (2.0)   ///< Release time in seconds after a note off event.
#define AUDIO_ENGINE_MIDI_CHANNEL_DRUMS      (9)     ///< MIDI channel that indicates drums/percussions.


#include <SequenceTrack.hpp>
#include <tsf.h>
#include <portaudio.h>


class AudioEngine {
    public:
        /**
         *  @brief Initialize the audio engine.
         *  @return True if success, false otherwise.
         */
        static bool Initialize(void);

        /**
         *  @brief Terminate the audio engine.
         */
        static void Terminate(void);

        /**
         *  @brief Render the sound of a sequence track.
         *  @param [in] track The sequence track for which to render the sound.
         */
        static void RenderSound(SequenceTrack& track);

        /**
         *  @brief Start the audio stream.
         *  @return True if success, false otherwise.
         */
        static bool StartStream(void);

        /**
         *  @brief Stop the audio stream.
         *  @return True if success, false otherwise.
         */
        static bool StopStream(void);

        /**
         *  @brief Check whether audio stream is playing.
         *  @return True if audio stream is playing, false otherwise.
         */
        static bool StreamIsPlaying(void);

        /**
         *  @brief Get the current time pointer (output latency corrected).
         *  @return Time pointer in seconds.
         */
        static double GetTimePointer(void);

        /**
         *  @brief Set a time pointer value.
         *  @param [in] timePointer Time pointer in seconds.
         *  @details The stream must be stopped, otherwise this function has no effect.
         */
        static void SetTimePointer(double timePointer);

    private:
        static bool initialized;       ///< True if audio engine is initialized, false otherwise.
        static tsf* soundFont;         ///< Sound font object (set during initialization).
        static PaStream* audioStream;  ///< Audio stream object (set during initialization).

        /* Timing properties */
        static std::chrono::time_point<std::chrono::steady_clock> timeOfStart;     ///< Time when the stream was started.
        static double outputLatency;      ///< Output latency in seconds. The value is obtained when the stream is started.
        static double timePointer;        ///< Time pointer to the current point of the song (zero indicates start of song).
        static double timePointerOfStart; ///< Time pointer value when stream was started.

        static int CallbackAudioStream(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData);
};

