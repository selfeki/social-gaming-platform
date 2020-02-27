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
    spec.configuration = parseConfig(j.at("configuration"));
    spec.gameState     = parseGameState(j);
    spec.rules         = parseRules(j.at("rules"));
    return spec;
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
    for(const auto& [key, val]: j.at("setup").items()) {
        setup.set(key, parseSetupValue(val));
    }
    return setup;
}

SetupValue
parseSetupValue(const json& j) {
    switch (j.type()) {
        case json::value_t::string:         return j.get<std::string>();
        case json::value_t::number_integer: return j.get<int>();
        case json::value_t::boolean:        return j.get<bool>();
        case json::value_t::object:         return parseCustomSetup(j);
        default: assert(false);             return 0; // todo: log error
    }
}

CustomSetup
parseCustomSetup(const json& j) {
    auto kind   = parseDataKind(j.at("kind"));
    auto prompt = j.at("prompt").get<std::string>();
    return {kind, prompt};
}

// Map for switch-case convenience
const std::unordered_map<std::string, DataKind> strToKind = {
    {"integer",         DataKind::INTEGER},
    {"string",          DataKind::STRING},
    {"boolean",         DataKind::BOOLEAN},
    {"question-answer", DataKind::QUESTION_ANSWER},
    {"multiple-choice", DataKind::MULTIPLE_CHOICE}
};

DataKind
parseDataKind(const json& j) {
    auto str = j.get<std::string>();
    auto it  = strToKind.find(str);
    if (it != strToKind.end()){
        assert(false);  // this should have been caught in validation
    }
    return it->second;
}

// todo refactor namespaces and class names for concision
GameState
parseGameState(const json& j) {
    GameState state;
    state.constants   = parseEnvironment(j.at("constants"));
    state.variables   = parseEnvironment(j.at("variables"));
    state.perPlayer   = parseEnvironment(j.at("per-player"));
    state.perAudience = parseEnvironment(j.at("per-audience"));
    return state;
}

Environment
parseEnvironment(const json& j) {
    Environment env;
    for(const auto& item: j.items()) {
        auto key   = item.key();
        auto value = item.value();
        auto exp   = parseExpression(value);
        env.set(key, exp);
    }
    return env;
}

Expression
parseExpression(const json& j) {
    switch (j.type()) {
        case json::value_t::string:         return j.get<std::string>();
        case json::value_t::number_integer: return j.get<int>();
        case json::value_t::boolean:        return j.get<bool>();
        case json::value_t::object:         return parseExpMap(j);
        case json::value_t::array:          return parseExpList(j);
        default: assert(false); // todo: log error
    }
}

Expression
parseExpMap(const json& j) {
    auto expMap = MapWrapper<std::string, Expression>();
    for(const auto& [key, val]: j.items()) {
        Expression exp = parseExpression(val);
        expMap.set(key, exp);
    }
    return expMap;
}

Expression
parseExpList(const json& j) {
    auto expList = std::vector<Expression>();
    for(const auto& [_, val]: j.items()) {
        // todo: change to std::transform
        // todo: test expressions are serialized in right order
        Expression exp = parseExpression(val);
        expList.emplace_back(exp);
    }
    return expList;
}

gameSpecification::rules::RuleList
parseRules(const json& j) {
    // todo
}


bool 
hasValidFields(const json& j) {
    return hasAllRequiredFields(j)
        && hasNoExtraFields(j);
}

bool 
hasAllRequiredFields(const json& j) {
    json flat = j.flatten();
    // change to accumulate?
    for (auto element: enum_to_str) {
        if (flat[element.second] == NULL) {
            return false;
        }
    }
    return true;
}

bool 
hasNoExtraFields(const json& j) {
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
