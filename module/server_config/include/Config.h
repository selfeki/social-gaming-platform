#pragma once

//#include "arepa/game_spec/GameSpecification.h"

#include <string>
#include <unordered_map>

namespace serverConfig {


enum FIELDS {
    PORT,
    HTML,
    GAMES
};

const std::unordered_map<FIELDS, std::string> enum_to_str = {
    { PORT, "/port" },
    { HTML, "/htmlpath" },
    { GAMES, "/games" }
};

struct Configuration {
    int port;
    std::string htmlPath;
    //    std::vector<gameSpecification::Specification> gameSpecs;

    bool err = true;    //default true, set only to false when fields are assigned to values
};


}    // namespace serverConfig
