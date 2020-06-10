#include <MIDIFile.hpp>


bool MIDIFile::Read(std::string filename){
    std::ifstream file(filename, std::ios::binary);
    file.unsetf(std::ios::skipws);
    if(!file.is_open())
        return false;
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> bytes;
    bytes.reserve(fileSize);
    bytes.insert(bytes.begin(), std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>());
    return this->Read(bytes);
}

bool MIDIFile::Read(std::vector<uint8_t>& bytes){
    // First chunk must be a header chunk "MThd"
    tracks.clear();
    if(bytes.size() < 14) return false;
    if(0x4D546864 != ((uint32_t(bytes[0]) << 24) | (uint32_t(bytes[1]) << 16) | (uint32_t(bytes[2]) << 8) | uint32_t(bytes[3]))) return false;
    if(0x00000006 != ((uint32_t(bytes[4]) << 24) | (uint32_t(bytes[5]) << 16) | (uint32_t(bytes[6]) << 8) | uint32_t(bytes[7]))) return false;
    if(!header.Decode(&bytes[8], 6)) return false;

    // Decode track chunks
    size_t index = 14;
    while((index + 7) < bytes.size()){
        uint32_t fourCC = (uint32_t(bytes[index]) << 24) | (uint32_t(bytes[index + 1]) << 16) | (uint32_t(bytes[index + 2]) << 8) | uint32_t(bytes[index + 3]);
        uint32_t length = (uint32_t(bytes[index + 4]) << 24) | (uint32_t(bytes[index + 5]) << 16) | (uint32_t(bytes[index + 6]) << 8) | uint32_t(bytes[index + 7]);
        index += 8;
        if((index + size_t(length)) > bytes.size()){
            tracks.clear();
            return false;
        }
        // "MTrk"
        if(0x4D54726B == fourCC){
            MIDIChunkTrack track;
            if(!track.Decode(&bytes[index], length)){
                tracks.clear();
                return false;
            }
            tracks.push_back(std::move(track));
        }
        // Alien chunks are ignored
        index += (size_t)length;
    }

    // Check if header was right
    if(header.numTracks != (uint32_t)tracks.size()){
        tracks.clear();
        return false;
    }

    // Compute absolute ticks (ticks per quarter-note format only)
    if(!(0x8000 & header.division)){
        for(auto&& track : tracks){
            uint64_t t = 0;
            for(auto&& e : track.events){
                e.absoluteTicks = (t += (uint64_t)e.deltaTime);
            }
        }
    }
    return true;
}

bool MIDIFile::Write(std::string filename){
    std::vector<uint8_t> bytes;
    if(!this->Write(bytes))
        return false;
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if(!file.is_open())
        return false;
    file.write((char*)&bytes[0],bytes.size());
    return true;
}

bool MIDIFile::Write(std::vector<uint8_t>& bytes){
    bytes.clear();
    if((uint16_t)tracks.size() != header.numTracks){
        LogError("MISMATCH!\n");
        return false;
    }

    // Header chunk
    bytes.push_back(0x4D);
    bytes.push_back(0x54);
    bytes.push_back(0x68);
    bytes.push_back(0x64);
    bytes.push_back(0x00);
    bytes.push_back(0x00);
    bytes.push_back(0x00);
    bytes.push_back(0x06);
    if(!header.Encode(bytes)){
        bytes.clear();
        return false;
    }

    // Data chunks
    for(auto&& track : tracks){
        std::vector<uint8_t> chunkData;
        if(!track.Encode(chunkData)){
            bytes.clear();
            return false;
        }
        bytes.push_back(0x4D);
        bytes.push_back(0x54);
        bytes.push_back(0x72);
        bytes.push_back(0x6B);
        uint32_t length = (uint32_t)chunkData.size();
        bytes.push_back((uint8_t)(0x000000FF & (length >> 24)));
        bytes.push_back((uint8_t)(0x000000FF & (length >> 16)));
        bytes.push_back((uint8_t)(0x000000FF & (length >> 8)));
        bytes.push_back((uint8_t)(0x000000FF & length));
        bytes.insert(bytes.end(), chunkData.begin(), chunkData.end());
    }
    return true;
}

uint32_t MIDIFile::ReadVariableLength(uint32_t& bytesRead, const uint8_t* bytes, uint32_t length){
    bytesRead = 0;
    uint32_t result = 0;
    for(int i = 0; (i < 4) && (bytesRead < length); i++){
        uint8_t b = bytes[bytesRead++];
        result <<= 7;
        result |= (b & 0x7F);
        if(!(0x80 & b)){
            break;
        }
    }
    return result;
}

