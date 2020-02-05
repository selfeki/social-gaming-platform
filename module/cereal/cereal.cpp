#include <iostream>
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

void to_json(json& j, const networking::Message& msg) { // automatically called when -> json j = msg
    //j = json{{json_fields[0], msg.text}};
}

void from_json(const json& j, networking::Message& msg) { // automatically called when -> networking::Message msg = j.get<networking::Message>()
    //j.at(json_fields[0]).get_to(msg.text);
}
