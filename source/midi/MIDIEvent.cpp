#include <MIDIEvent.hpp>


MIDIEvent::MIDIEvent(){
    deltaTime = 0;
    status = 0;
    absoluteTicks = 0;
}

MIDIEvent::MIDIEvent(uint32_t deltaTime, uint8_t status){
    this->deltaTime = deltaTime;
    this->status = status;
}

MIDIEvent::MIDIEvent(uint32_t deltaTime, uint8_t status, const uint8_t* data, const uint32_t datalen){
    this->deltaTime = deltaTime;
    this->status = status;
    this->data.assign(data, data + datalen);
}

