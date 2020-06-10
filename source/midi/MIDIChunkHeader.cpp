#include <MIDIChunkHeader.hpp>


MIDIChunkHeader::MIDIChunkHeader(){
    format = 0;
    numTracks = 0;
    division = 0;
}

bool MIDIChunkHeader::Decode(const uint8_t* chunkData, const uint32_t length){
    if(6 != length) return false;
    this->format = ((uint16_t)chunkData[0] << 8) | (uint16_t)chunkData[1];
    this->numTracks = ((uint16_t)chunkData[2] << 8) | (uint16_t)chunkData[3];
    this->division = ((uint16_t)chunkData[4] << 8) | (uint16_t)chunkData[5];
    return true;
}

bool MIDIChunkHeader::Encode(std::vector<uint8_t>& bytes){
    bytes.push_back((uint8_t)(0x00FF & (this->format >> 8)));
    bytes.push_back((uint8_t)(0x00FF & this->format));
    bytes.push_back((uint8_t)(0x00FF & (this->numTracks >> 8)));
    bytes.push_back((uint8_t)(0x00FF & this->numTracks));
    bytes.push_back((uint8_t)(0x00FF & (this->division >> 8)));
    bytes.push_back((uint8_t)(0x00FF & this->division));
    return true;
}

