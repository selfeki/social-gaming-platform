#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include "Server.h"

using json = nlohmann::json;

namespace formats {

    //Message & Json serializer, deserializer
    void to_json(json& j, const networking::Message& msg);

    void from_json(const json& j, networking::Message& msg);

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