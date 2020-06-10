#pragma once


#include <MIDIChunkHeader.hpp>
#include <MIDIChunkTrack.hpp>


class MIDIFile {
    public:
        MIDIChunkHeader header;                ///< Header chunk.
        std::vector<MIDIChunkTrack> tracks;    ///< All track chunks.

        /**
         *  \brief Read MIDI data from MIDI file.
         *  \param [in] filename Name of the MIDI file to read.
         *  \return True if success, false otherwise.
         */
        bool Read(std::string filename);

        /**
         *  \brief Read MIDI data from binary data of MIDI file.
         *  \param [in] bytes Binary MIDI file data.
         *  \return True if success, false otherwise.
         */
        bool Read(std::vector<uint8_t>& bytes);

        /**
         *  @brief Write MIDI data to a MIDI file.
         *  @param [in] filename Name of the MIDI file to write.
         *  @return True if success, false otherwise.
         */
        bool Write(std::string filename);

        /**
         *  @brief Write MIDI data to binary data vector.
         *  @param [out] bytes Container where to store the binary data.
         *  @return True if success, false otherwise.
         */
        bool Write(std::vector<uint8_t>& bytes);

        /**
         *  @brief Read variable length value from binary data.
         *  @param [out] bytesRead Number of bytes that have been read.
         *  @param [in] bytes Bytes that contain the variable length value.
         *  @param [in] length Number of bytes in the buffer.
         *  @return The decoded variable length value.
         */
        static uint32_t ReadVariableLength(uint32_t& bytesRead, const uint8_t* bytes, uint32_t length);
};

