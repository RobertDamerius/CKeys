#include <SequenceTrack.hpp>


SequenceTrack::SequenceTrack(uint8_t channel, std::string name){
    this->channel = channel;
    this->name = name;
    this->instrumentType = 0;
    this->colorWhiteKey = glm::u8vec3(145,222,64);
    this->colorBlackKey = glm::u8vec3(94,154,27);
}

void SequenceTrack::SetColor(uint32_t value){
    switch(value % 7){
        case 0:
            this->colorWhiteKey = glm::u8vec3(145,222,64);
            this->colorBlackKey = glm::u8vec3(94,154,27);
            break;
        case 1:
            this->colorWhiteKey = glm::u8vec3(116,155,196);
            this->colorBlackKey = glm::u8vec3(62,101,145);
            break;
        case 2:
            this->colorWhiteKey = glm::u8vec3(237,177,32);
            this->colorBlackKey = glm::u8vec3(151,109,13);
            break;
        case 3:
            this->colorWhiteKey = glm::u8vec3(217,83,25);
            this->colorBlackKey = glm::u8vec3(156,60,18);
            break;
        case 4:
            this->colorWhiteKey = glm::u8vec3(126,47,142);
            this->colorBlackKey = glm::u8vec3(84,31,95);
            break;
        case 5:
            this->colorWhiteKey = glm::u8vec3(162,20,47);
            this->colorBlackKey = glm::u8vec3(131,16,39);
            break;
        case 6:
            this->colorWhiteKey = glm::u8vec3(0,114,189);
            this->colorBlackKey = glm::u8vec3(0,91,153);
            break;
    }
}

