#pragma once


class MIDIChunkHeader {
    public:
        uint16_t format;      ///< The MIDI format. Valid formats are 0, 1 and 2.
        uint16_t numTracks;   ///< Number of tracks.
        uint16_t division;    ///< Defines the default unit of delta-time.

        /**
         *  \brief Create a MIDI header chunk.
         */
        MIDIChunkHeader();

        /**
         *  \brief Decode chunk data of a header chunk.
         *  \param [in] chunkData Chunk data to be decoded.
         *  \param [in] length Number of bytes to be decoded.
         *  \return True if success, false otherwise.
         */
        bool Decode(const uint8_t* chunkData, const uint32_t length);

        /**
         *  @brief Encode header chunk.
         *  @param [out] bytes Container to which to append the encoded bytes.
         *  @return True if success, false otherwise.
         */
        bool Encode(std::vector<uint8_t>& bytes);
};

