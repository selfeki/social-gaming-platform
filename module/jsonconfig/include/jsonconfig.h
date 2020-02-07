#pragma once

#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include <list>
using json = nlohmann::json;

namespace game_config {

    struct configuration {
        std::string name;
        std::unordered_map<std::string, json> player_count;
        bool audience;
        std::unordered_map<std::string, json> setup;
        std::unordered_map<std::string, json> constants;
        std::unordered_map<std::string, json> variables;
        std::unordered_map<std::string, json> per_player;
        std::unordered_map<std::string, json> per_audience;
        std::vector<json> rules;
    };

    void from_json(const json&, configuration&);

    //Returns an empty vector on failure to find rule.
    //Returns a vector of all json objects that matches rule name
    std::vector<json> find_rule(const std::vector<json>&, const std::string&);

    void print_config(const configuration&); // exists as an example for accessing data
}

namespace server_config{

    struct configuration {
        int port;
        std::string htmlpath;
    };
    void from_json(const json&, configuration&);

    void print_config(const configuration&);

}
//TODO: add server configuration fields and serializer