#pragma once


#include <MIDIEvent.hpp>
#include <NoteBlock.hpp>


/* Forward declaration of friendly class */
class Sequencer;


class SequenceTrack {
    public:
        uint8_t channel;                              ///< MIDI channel of the track.
        std::string name;                             ///< Name of the track.
        uint8_t instrumentType;                       ///< The LATEST instrument type of the track (latest program change MIDI message).
        std::array<std::vector<NoteBlock>, 88> lanes; ///< 88 lanes where each lane can contain different numbers of blocks. The note blocks are sorted by time.
        glm::u8vec3 colorWhiteKey;                    ///< Display color for white keys.
        glm::u8vec3 colorBlackKey;                    ///< Display color for black keys.
        std::vector<float> samples;                   ///< Samples of a stereo sound for the whole track. Those values are calculated if this sequence track object is passed to the sound rendering function of the audio engine.

        /**
         *  @brief Create a sequence track.
         *  @param [in] channel The MIDI channel of the sequence track, defaults to 0.
         *  @param [in] name The track name, defaults to "".
         */
        explicit SequenceTrack(uint8_t channel = 0, std::string name = std::string(""));

        /**
         *  @brief Set a default color based on an integer value.
         *  @param [in] value Integer value that is used to set deterministic color value.
         */
        void SetColor(uint32_t value);

    protected:
        friend Sequencer;
        std::vector<MIDIEvent> midiEvents;                           ///< MIDI events containing only note on/off events.
        std::vector<std::pair<uint64_t, bool>> sustainPedalChanges;  ///< Sustain pedal changes. First: absolute ticks, second: pedal pressed or not.
};

