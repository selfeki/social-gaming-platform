#include "jsonSerializer.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>


using json = nlohmann::json;


namespace jsonSerializer {

using namespace serverConfig;
using namespace gameSpecification;

// Server configuration serializing

serverConfig::Configuration
parseServerConfig(const json& j) {
    if (!isValidServerConfig(j)) {
        throw;
    }    // todo: handle error gracefully
    serverConfig::Configuration config;
    j.at("port").get_to(config.port);
    j.at("html").get_to(config.htmlPath);
    auto games = j.at("games");
    std::transform(
        games.begin(), games.end(), std::back_inserter(config.gameSpecs),
        [](const json& spec) { return parseGameSpecification(spec); });
    return config;
}

// Note: Only validates top-level keys of server config
bool isValidServerConfig(const json& j) {
    json temp = j.flatten();
    for (auto element : serverConfig::enum_to_str) {
        if (temp[element.second] == NULL) {
            return false;
        }
    }
    return true;
}

// Game specification serializing

gameSpecification::Specification
parseGameSpecification(const json& j) {
    if (!isValidGameSpec(j)) {
        throw;
    }    // todo: handle error gracefully
    Specification spec;
    spec.configuration = parseConfig(j.at("configuration"));
    spec.gameState = parseGameState(j);
    spec.rules = parseRules(j.at("rules"));
    return spec;
}

gameSpecification::Configuration
parseConfig(const json& j) {
    gameSpecification::Configuration config;
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
    for (const auto& [key, val] : j.at("setup").items()) {
        setup.set(key, parseSetupValue(val));
    }
    return setup;
}

SetupValue
parseSetupValue(const json& j) {
    switch (j.type()) {
    case json::value_t::string:
        return j.get<std::string>();
    case json::value_t::number_integer:
        return j.get<int>();
    case json::value_t::boolean:
        return j.get<bool>();
    case json::value_t::object:
        return parseCustomSetup(j);
    default:
        assert(false);
        return 0;    // todo: log error
    }
}

CustomSetup
parseCustomSetup(const json& j) {
    auto kind = parseDataKind(j.at("kind"));
    auto prompt = j.at("prompt").get<std::string>();
    return { kind, prompt };
}

// Map for switch-case convenience
const std::unordered_map<std::string, DataKind> strToKind = {
    { "integer", DataKind::INTEGER },
    { "string", DataKind::STRING },
    { "boolean", DataKind::BOOLEAN },
    { "question-answer", DataKind::QUESTION_ANSWER },
    { "multiple-choice", DataKind::MULTIPLE_CHOICE }
};

DataKind
parseDataKind(const json& j) {
    auto str = j.get<std::string>();
    auto it = strToKind.find(str);
    if (it != strToKind.end()) {
        assert(false);    // this should have been caught in validation
    }
    return it->second;
}

// todo refactor namespaces and class names for concision
GameState
parseGameState(const json& j) {
    GameState state;
    state.constants = parseEnvironment(j.at("constants"));
    state.variables = parseEnvironment(j.at("variables"));
    state.perPlayer = parseEnvironment(j.at("per-player"));
    state.perAudience = parseEnvironment(j.at("per-audience"));
    return state;
}

Environment
parseEnvironment(const json& j) {
    Environment env;
    for (const auto& item : j.items()) {
        auto key = item.key();
        auto value = item.value();
        auto exp = parseExpression(value);
        env.set(key, exp);
    }
    return env;
}

Expression
parseExpression(const json& j) {
    switch (j.type()) {
    case json::value_t::string:
        return j.get<std::string>();
    case json::value_t::number_integer:
        return j.get<int>();
    case json::value_t::boolean:
        return j.get<bool>();
    case json::value_t::object:
        return parseExpMap(j);
    case json::value_t::array:
        return parseExpList(j);
    default:
        assert(false);    // todo: log error
    }
}

Expression
parseExpMap(const json& j) {
    auto expMap = MapWrapper<std::string, Expression>();
    for (const auto& [key, val] : j.items()) {
        Expression exp = parseExpression(val);
        expMap.set(key, exp);
    }
    return expMap;
}

Expression
parseExpList(const json& j) {
    auto expList = std::vector<Expression>();
    for (const auto& [_, val] : j.items()) {
        // todo: change to std::transform
        // todo: test expressions are serialized in right order
        Expression exp = parseExpression(val);
        expList.emplace_back(exp);
    }
    return expList;
}

rule::RuleList
parseRules(const json& j) {
    // todo
}

bool isValidGameSpec(const json& j) {
    return (hasAllRequiredFields(j)
        && hasNoExtraFields(j)
        && validFieldValues(j));
}

bool hasNoExtraFields(const json& j) {
    // json duplicate keys do not exist. Check to see if all field counts match
    int num_fields = FIELDS.size();
    int num_fields_configuration = FIELDS_configuration.size();
    int num_fields_playerCount = FIELDS_playerCount.size();
    bool eq_topFields = (num_fields == j.size());
    bool eq_configFields = (num_fields_configuration == j.at("configuration").size());
    bool eq_playerCountFields = (num_fields_playerCount == ((j.at("configuration")).at("player count")).size());
    if (eq_topFields && eq_configFields && eq_playerCountFields) {
        //all fields are accounted for. No extra fields
        return true;
    }
    return false;
}

bool hasAllRequiredFields(const json& j) {
    //check json contains all top level fields
    std::map<std::string,json::value_t>::const_iterator it;
    it = find_if(FIELDS.begin(), FIELDS.end(), [&j](const std::pair<std::string,json::value_t>& target) {
        return !j.contains(target.first);
    });
    if (it != FIELDS.end()) {
        //a field is missing from json object
        return false;
    }
    
    //check json contains all subfields
    //subfield check: "configuration" contains all fields
    json config = j.at("configuration");
    it = find_if(FIELDS_configuration.begin(), FIELDS_configuration.end(), [&config](const std::pair<std::string,json::value_t>& target) {
        return !config.contains(target.first);
    });
    if (it != FIELDS_configuration.end()) {
        //a field is missing from json object
        return false;
    }
    
    //subfield check: "configuration"->"player count" contains all fields
    json playerCount = (j.at("configuration")).at("player count");
    it = find_if(FIELDS_playerCount.begin(), FIELDS_playerCount.end(),[&playerCount](const std::pair<std::string,json::value_t>& target) {
        return !playerCount.contains(target.first);
    });
    if (it != FIELDS_playerCount.end()) {
        //a field is missing from json object
        return false;
    }

    return true;
}

bool validFieldValues(const json& j) {
    //check json contains all valid top level values
    for(auto& item : j.items()) {
        std::string key = item.key();
        json::value_t value = item.value().type();
        if(value != FIELDS.at(key)) {
            return false;
        }
    }

    //check json contains all valid configuration values
    for(auto& item : j["configuration"].items()) {
        std::string key = item.key();
        json::value_t value = item.value().type();
        if(value != FIELDS_configuration.at(key)) {
            return false;
        }
    }
    
    //check json contains all valid player count values
    for(auto& item : j["configuration"]["player count"].items()) {
        std::string key = item.key();
        json::value_t value = item.value().type();
        if(value != FIELDS_playerCount.at(key)) {
            return false;
        }
    }

    return true;
}


}