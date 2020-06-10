#pragma once


#include <SequenceTrack.hpp>


#define SEQUENCER_TEMPO_SCALE_MIN    (0.5)
#define SEQUENCER_TEMPO_SCALE_MAX    (2.0)


class Sequencer {
    public:
        std::string name;                        ///< Name of the sequence (is set when @ref ReadMIDIFile is called).
        std::vector<SequenceTrack> tracks;       ///< List of all sequence tracks.

        /* Audio streaming attributes */
        uint32_t currentSample;                  ///< The current sample index of the streaming buffer. This value is used by the audio engine. NOTE: This is the index to a stereo sample buffer (0,2,4,.. indicate left samples, 1,3,5,.. indicate right samples).
        uint32_t maxNumSamples;                  ///< The greatest number of samples of all @ref tracks. This value is set by the @ref Generate member function. NOTE: This is the length of stereo sample buffer (number of all floats).

        /**
         *  @brief Create an empty sequencer.
         */
        Sequencer();

        /**
         *  @brief Read a MIDI file.
         *  @param [in] filename The filename of the MIDI file to be opened.
         *  @return True if success, false otherwise.
         *  @details Reads a MIDI file and checks for supported formats.
         */
        bool ReadMIDIFile(std::string filename);

        /**
         *  @brief Generate or re-generate all sequence @ref tracks from the last MIDI file read and also (re-)generate audio samples.
         *  @param [in] tempoScale Scaling for tempo of the whole sequence in range [SEQUENCER_TEMPO_SCALE_MIN, SEQUENCER_TEMPO_SCALE_MAX].
         */
        void Generate(double tempoScale = 1.0);

    private:
        uint32_t ticksPerQuarter;                              ///< Number of ticks per quarter note.
        std::vector<std::pair<uint64_t, double>> tempoChanges; ///< Absolute ticks where tempo changes occur (seconds per quarter note).

        /**
         *  @brief Convert a tick value to a timestamp in seconds.
         *  @param [in] tick Absolute tick value of a MIDI event.
         *  @return Corresponding absolute timestamp in seconds.
         */
        double GetTimestamp(uint64_t tick);

        /**
         *  @brief Get the state of the sustain pedal for the current absolute tick value.
         *  @param [in] tick Absolute tick value at which to query the pedal state.
         *  @param [in] sustainPedalChanges Vector of pedal state changes.
         *  @return True if pedal is pressed at query tick, false otherwise.
         */
        bool GetPedalState(uint64_t tick, std::vector<std::pair<uint64_t, bool>>& sustainPedalChanges);

        /**
         *  @brief Get the timestamp of the next pedal off event.
         *  @param [in] tick Absolute tick value from which to start the search.
         *  @param [in] sustainPedalChanges Vector of pedal state changes.
         *  @return Corresponnding absolute timestamp in seconds or INF if sustain pedal is never released.
         */
        double GetTimestampOfNextPedalOff(uint64_t tick, std::vector<std::pair<uint64_t, bool>>& sustainPedalChanges);
};

