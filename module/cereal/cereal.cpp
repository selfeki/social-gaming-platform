#include <stdio.h>
#include "cereal.h"
#include "Server.h"
#include <iostream>

Cereal::Cereal(networking::Message m) : msg(m) {
    formats::to_json(j, msg);
}

json Cereal::getJson() const {
    return j;
}

networking::Message Cereal::getMessage() const {
    return msg;
}

void Cereal::printMessage() const {
    std::cout<<"message:"<<msg.text<<std::endl;
}
