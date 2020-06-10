#pragma once


#include <SequenceTrack.hpp>


class Recorder {
    public:
        SequenceTrack track; ///< Track data for visualization.
        std::mutex mtxTrack; ///< Protect @ref track.
        std::vector<std::pair<double, std::vector<unsigned char>>> rawRecordedData; ///< Raw MIDI data received during recording (delta time [s], data bytes).

        /**
         *  @brief Create an empty recorder.
         */
        Recorder();

        /**
         *  @brief Delete the recorder object.
         */
        ~Recorder();

        /**
         *  @brief Get the current time pointer.
         *  @return Current time pointer in seconds.
         */
        double GetTimePointer(void);

        /**
         *  @brief Start recording.
         *  @return True if success, false otherwise.
         *  @details The @ref track is cleared before the actual recording is started.
         */
        bool StartRecording(void);

        /**
         *  @brief Stop recording.
         */
        void StopRecording(void);

        /**
         *  @brief Check whether the recorder is in recording mode or not.
         *  @return True if recording, false otherwise.
         */
        inline bool IsRecording(void){ return (nullptr != midiIn); }

        /**
         *  @brief Save recording to a MIDI file.
         *  @return True if success, false otherwise.
         */
        bool Save(void);

    private:
        RtMidiIn* midiIn;                                                ///< MIDI input object.
        uint8_t runningStatus;                                           ///< Running status byte (latest status).
        std::chrono::time_point<std::chrono::steady_clock> timeOfStart;  ///< Time when first MIDI message was received.
        std::atomic<bool> isRecording;                                   ///< True if actual recording has been started, false otherwise.
        double latestTimePointer;                                        ///< The latest timepointer.

        /**
         *  @brief Callback function that receives MIDI data.
         *  @param [in] timestamp Timestamp in seconds.
         *  @param [in] message Received MIDI message.
         */
        void ReceiveMIDI(double timestamp, std::vector<unsigned char>& message);
        static void CallbackMidiIn(double timestamp, std::vector<unsigned char> *message, void *userData){ ((Recorder*)userData)->ReceiveMIDI(timestamp, *message); }
};

