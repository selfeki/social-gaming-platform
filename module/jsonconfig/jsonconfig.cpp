#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "jsonconfig.h"
#include "jsonSerializerImpl.h"

using namespace json_serializer;
using namespace gameSpecification;

using json = nlohmann::json;


gameSpecification::Specification
parseGameSpecification(const json& j) {
    if (!hasValidFields(j)) { 
        throw;
    } // todo: handle error gracefully

    Specification spec;
    from_json(j, spec);
    return spec;
}

void 
from_json(const json& j, gameSpecification::Specification& spec) {
    auto config = parseConfig(j.at("configuration"));
    auto state  = parseStateSchema(j);
    auto rules  = parseRules(j.at("rules"));
}

gameSpecification::Configuration 
parseConfig(const json& j) {
    gameSpecification::Configuration  config;
    j.at("name").get_to(config.name);
    j.at("player count").at("min").get_to(config.count.min);
    j.at("player count").at("max").get_to(config.count.max);
    j.at("audience").get_to(config.allowAudience);
    config.setup = parseSetup(j);
    return config;
}

gameSpecification::Setup
parseSetup(const json& j) {
    Setup setup;
    from_json(j.at("setup"), setup);
    return setup;
}

void 
from_json(const json& j, gameSpecification::Setup& setup) {
    for(const auto& [key, val]: j.items()) {
        setup.set(key, parseSetupValue(val));
    }
}

SetupValue
parseSetupValue(const json& j) {
    switch (j.type()) {
        case json::value_t::string: return j.get<std::string>();
        case json::value_t::number_integer: return j.get<int>();
        case json::value_t::boolean: return j.get<bool>();
        case json::value_t::object: return parseCustomSetup(j);
        default: throw std::runtime_error(""); // todo
    }
}

CustomSetup
parseCustomSetup(const json& j) {
    auto kind = parseDataKind(j.at("kind"));
    auto prompt = j.at("prompt").get<std::string>();
    return {kind, prompt};
}

DataKind
parseDataKind(const json& j) {
    // todo
}

// gameSpecification::Expression serializer
void from_json(const json& j, gameSpecification::Expression& exp) {
    // todo: serialize boost:variant
}

// todo refactor namespaces and class names for concision
gameSpecification::GameState
parseStateSchema(const json& j) {
    gameSpecification::GameState state;
    // j.at("constants").get_to(state.constants);
    // j.at("variables").get_to(state.variables);
    // j.at("per-player").get_to(state.perPlayer);
    // j.at("per-audience").get_to(state.perAudience);
    return state;
}

gameSpecification::rules::RuleList
parseRules(const json& j) {
    
}


bool hasValidFields(const json& j) {
    return hasAllRequiredFields(j)
        && hasNoExtraFields(j);
}

bool hasAllRequiredFields(const json& j) {
    json flat = j.flatten();
    // change to accumulate?
    for (auto element: enum_to_str) {
        if (flat[element.second] == NULL) {
            return false;
        }
    }
    return true;
}

bool hasNoExtraFields(const json& j) {
    // need to check for duplicate keys, as that would be invalid
}


// std::vector<json> find_rule(const std::vector<json>& j, const std::string& rule_name) {
//     std::vector<json> ret;
//     for (auto& element: j) {
//         auto found = element.find("rule");
//         if (found != element.end() && element.at("rule") == rule_name) {
//             ret.push_back(element);
//         }
//     }
//     return ret;
// }

// json first_rule(const json& j, const int i) {
//     if (j.find("rules") == j.end()) {
//         return j;
//     } else {
//         return j["rules"][i];
//     }
// }

// bool has_rules(const json& j) {
//     return j.find("rules") != j.end();
// }

// void print_spec(const specification& spec) {

//     std::cout << "specification:\n"
//     << "name: " << spec.name
//     //<< "\nplayer count: " << spec.player_count
//     << "\nPlayer count: Min: "<< spec.player_count_s.min <<"Max: "<< spec.player_count_s.max
//     << "\naudience: " << spec.audience
//     //<< "\nsetup: " << spec.setup
//     << "\nsetup: " << spec.setup_s.rounds
//     //<< "\nconstants: " << spec.constants
//     //<< "\nvariables: " << spec.variables
//     //<< "\nper_player: " << spec.per_player
//     << "\nwins per player : "<< spec.per_player_s.wins
//     << "\nper_audience: " << spec.per_audience
//     << "\nweapons: \n";
//     for (const auto& elem: spec.constants_s.weapons){
//         std::cout << "\tname: " << elem.name << " beats: " << elem.beats << "\n";
//     }
//     std::cout << "\nrules: ";
//     for (const auto& element: spec.rules) {
//         std::cout << element << "\n\n";
//     }

//     std::cout << "\n";
// }

// specification load_file(const std::string& filepath, bool debug) {
//     json temp;
//     json_game_spec::specification ret;
//     std::ifstream s(filepath);
//     if (s.fail()) {
//         std::cerr << "Error: " << strerror(errno) << std::endl;
//         return ret;
//     }

//     try {
//         temp = json::parse(s);

//         ret = temp.get<json_game_spec::specification>();
//     }
//     catch (json::parse_error& e) {
//         std::cerr << "Failed to parse JSON.\n in jsonspec.cpp\n"
//         << "message: " << e.what();
//         return ret;
//     }


//     ret.jsonpath = filepath;
//     ret.err = false;
//     if (debug) {
//         json_game_spec::print_spec(ret);
//     }

//     return ret;
// }

// // //Server config
// void server_config::from_json(const json& j, server_config::configuration& config) {
//     j["port"].get_to(config.port);
//     j["html"].get_to(config.htmlpath);
//     config.err = false;
// }

// void server_config::print_config(const server_config::configuration& config) {
//     std::cout << "configuration: {\n"
//     << "html: " << config.htmlpath
//     << "\nport: " << config.port
//     << "\n}\n";
// }

// server_config::configuration server_config::load_file(const std::string& filepath, bool debug) {
//     json temp;
//     server_config::configuration ret;
//     std::ifstream s(filepath);
//     if (s.fail()) {
//         std::cerr << "Error: " << strerror(errno) << std::endl;
//         return ret;
//     }

//     try {
//         temp = json::parse(s);
//         ret = temp.get<server_config::configuration>();
//     }
//     catch (json::parse_error& e) {
//         std::cerr << "Failed to parse JSON.\n in jsonconfig.cpp\n"
//         << "message: " << e.what();
//         return ret;
//     }


//     ret.jsonpath = filepath;
//     ret.err = false;
//     if (debug) {
//         server_config::print_config(ret);
//     }

//     return ret;
// }

// bool server_config::valid(const json& j) {
//     json temp = j.flatten();
//     for (auto element: server_config::enum_to_str) {
//         if (temp[element.second] == NULL) {
//             return false;
//         }
//     }
//     return true;
// }
