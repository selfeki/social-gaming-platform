#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include "Server.h"

using json = nlohmann::json;

namespace formats {

    //Message & Json serializer, deserializer
    std::string json_fields[]{"message"};
    void to_json(json& j, const networking::Message& msg) { // automatically called when -> json j = msg
        j = json{{json_fields[0], msg.text}};
    }

    void from_json(const json& j, networking::Message& msg) { // automatically called when -> networking::Message msg = j.get<networking::Message>()
        j.at(json_fields[0]).get_to(msg.text);
        
    }

    // add more to and form functions ...

}

// goal to contain a message in all format
class Cereal {
    public:
        Cereal(networking::Message);
        json getJson() const;
        networking::Message getMessage() const;
        void printMessage() const;

    private:
        networking::Message msg;
        json j;
};