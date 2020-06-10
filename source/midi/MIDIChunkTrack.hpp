#pragma once


#include <MIDIEvent.hpp>


class MIDIChunkTrack {
    public:
        std::vector<MIDIEvent> events;

        /**
         *  \brief Decode chunk data of a track chunk.
         *  \param [in] chunkData Chunk data to be decoded.
         *  \param [in] length Number of bytes to be decoded.
         *  \return True if success, false otherwise.
         */
        bool Decode(const uint8_t* chunkData, const uint32_t length);

        /**
         *  @brief Encode data chunk.
         *  @param [out] bytes Container to which to append the encoded bytes.
         *  @return True if success, false otherwise.
         */
        bool Encode(std::vector<uint8_t>& bytes);

        /**
         *  @brief Get the sequence or the track name.
         *  @return The sequence or track name.
         */
        std::string GetSequenceTrackName(void);
};

