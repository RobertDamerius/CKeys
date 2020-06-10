#pragma once


class MIDIEvent {
    public:
        uint32_t deltaTime;          ///< Delta time that has to elapse after the previous event before this event happens. The format depends on the division attribute of the MIDI header (either ticks per quarter-node or SMPTE timecode).
        uint8_t status;              ///< Status byte of the event. For meta events this will be 0xFF and @ref data will contain the remaining bytes of the event.
        std::vector<uint8_t> data;   ///< Data bytes of the event.
        uint64_t absoluteTicks;      ///< This value is not part of the event and is computed from delta times of all previous events (ticks per quarter-note format only).

        /**
         *  @brief Create an empty MIDI event.
         */
        MIDIEvent();

        /**
         *  @brief Create a MIDI event without any data.
         *  @param [in] deltaTime Delta time of MIDI event.
         *  @param [in] status Status byte of MIDI event.
         */
        MIDIEvent(uint32_t deltaTime, uint8_t status);

        /**
         *  @brief Create a MIDI event.
         *  @param [in] deltaTime Delta time of MIDI event.
         *  @param [in] status Status byte of MIDI event.
         *  @param [in] data Buffer containing the data of the MIDI event.
         *  @param [in] datalen Actual length of the MIDI event data.
         */
        MIDIEvent(uint32_t deltaTime, uint8_t status, const uint8_t* data, const uint32_t datalen);
};

