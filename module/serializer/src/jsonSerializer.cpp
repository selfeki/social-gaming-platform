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

rules::RuleList
parseRules(const json& j) {

    std::vector<json> jsonRules = j["rules"].get<std::vector<json>>();
    for (auto rule : jsonRules){
        //rule.at("rule")
        if(rule.at("rule") == "foreach")
            std::cout << "HI IM AT FOREACH\n";
        else if (rule.at("rule") == "loop")
            std::cout << "HI IM AT scores\n";
        else if (rule.at("rule") == "inparallel")
            std::cout << "HI IM AT scores\n";
        else if (rule.at("rule") == "parallelfor")
            std::cout << "HI IM AT scores\n";
        else if (rule.at("rule") == "switch")
            std::cout << "HI IM AT scores\n";
        else if (rule.at("rule") == "when")
            std::cout << "HI IM AT scores\n";
        else if (rule.at("rule") == "extend")
            std::cout << "HI IM AT scores\n";
        else if (rule.at("rule") == "reverse")
            std::cout << "HI IM AT scores\n";
        else if (rule.at("rule") == "shuffle")
            std::cout << "HI IM AT scores\n";
        else if (rule.at("rule") == "sort")
            std::cout << "HI IM AT scores\n";
        else if (rule.at("rule") == "deal")
            std::cout << "HI IM AT scores\n";
        else if (rule.at("rule") == "discard")
            std::cout << "HI IM AT scores\n";
        else if (rule.at("rule") == "add")
            std::cout << "HI IM AT scores\n";
        else if (rule.at("rule") == "input-choice")
            std::cout << "HI IM AT scores\n";
        else if (rule.at("rule") == "input-text")
            std::cout << "HI IM AT scores\n";
        else if (rule.at("rule") == "input-vote")
            std::cout << "HI IM AT scores\n";
        else if (rule.at("rule") == "message")
            std::cout << "HI IM AT scores\n";
        else if (rule.at("rule") == "global-message")
            std::cout << "HI IM AT scores\n";
        else if (rule.at("rule") == "scores")
            std::cout << "HI IM AT scores\n";
    }
    /*
    auto innerJson = jsonRules[0].get<std::map<std::string, json>>();
    std::cout << "top level \n";
    for (auto i : jsonRules){
        std::cout << "\n\n" << i << "\n\n";
    }

    std::cout << "one down level \n";
    for (auto i : innerJson){
        std::cout << "\n\n" << i << "\n\n";
    }
    if (innerJson.at("rules").is_array()){
        std::cout << "LOOK AT MEEEE IM AN OBJECT";
    }
    for (auto subrule: innerJson.at("rules")){
        std::cout << subrule;
    }*/
    return std::vector<rules::Rule>();

}

bool isValidGameSpec(const json& j) {
    return hasAllRequiredFields(j)
        && hasNoExtraFields(j);
}

bool hasAllRequiredFields(const json& j) {
    json flat = j.flatten();
    // use std::all_of or any_of to implement this
    for (auto element : enum_to_str) {
        if (flat[element.second] == NULL) {
            return false;
        }
    }
    return true;
}

bool hasNoExtraFields(const json& j) {
    // need to check for duplicate keys, as that would be invalid
}


}
