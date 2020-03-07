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
using gameRule = gameSpecification::rule::Rule;
using gameRuleList = gameSpecification::rule::RuleList;

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
    j["port"].get_to(config.port);
    j["html"].get_to(config.htmlPath);
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
    spec.configuration = parseConfig(j["configuration"]);
    spec.gameState = parseGameState(j);
    //spec.rules = ruleSelector(j["rules"]);
    return spec;
}

gameSpecification::Configuration
parseConfig(const json& j) {
    gameSpecification::Configuration config;
    j["name"].get_to(config.name);
    j["player count"]["min"].get_to(config.count.min);
    j["player count"]["max"].get_to(config.count.max);
    j["audience"].get_to(config.allowAudience);
    config.setup = parseSetup(j);
    return config;
}

gameSpecification::Setup
parseSetup(const json& j) {
    Setup setup;
    for (const auto& [key, val] : j["setup"].items()) {
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
    state.constants = parseEnvironment(j["constants"]);
    state.variables = parseEnvironment(j["variables"]);
    state.perPlayer = parseEnvironment(j["per-player"]);
    state.perAudience = parseEnvironment(j["per-audience"]);
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

gameSpecification::Expression
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

gameSpecification::Expression
parseExpMap(const json& j) {
    auto expMap = MapWrapper<std::string, Expression>();
    for (const auto& [key, val] : j.items()) {
        Expression exp = parseExpression(val);
        expMap.set(key, exp);
    }
    return expMap;
}

gameSpecification::Expression
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

//all other parseRule__RULENAME__ will be similar with different fields
gameRule
parseRuleForEach(const json& j, gameRule& parent){
    gameSpecification::Expression list = parseExpression(j.value("list", ""));
    gameSpecification::Expression elem = parseExpression(j.value("element", ""));
    //std::cout << boost::get<std::string>(list);

    gameRuleList parsedSubRules = gameSpecification::rule::RuleList();
    gameSpecification::rule::ForEach forEachRule = {parent, list, elem, parsedSubRules};


    auto subRules = j["rules"].get<std::vector<json>>();
        //TODO: call ruleSelector on each of the rules of THIS rule
        //and push into RulesList of THIS rule
        //ie: global-message, parallelfor, etc in the rock paper scissors example
    //for each subrule
    return forEachRule;
}

//think of this as the phone book of rules
gameRule
ruleSelector(const json& j, std::string ruleName, gameRule& parent) {
        if (ruleName == "foreach")
            return parseRuleForEach(j, parent);
        /*
        else if (rule_type == "loop")
            std::cout << "placeholder";
            //TODO: implement parseLoopRules
        else if (rule_type == "inparallel")
            std::cout << "placeholder";
            //TODO: implement parse _ Rules
        else if (rule_type == "parallelfor")
            std::cout << "placeholder";
            //TODO: implement parse parallelfor Rules
        else if (rule_type == "switch")
            std::cout << "placeholder";
            //TODO: implement parse switch Rules
        else if (rule_type == "when")
            std::cout << "placeholder";
            //TODO: implement parse when Rules
        else if (rule_type == "extend")
            std::cout << "placeholder";
            //TODO: implement parse extend Rules
        else if (rule_type == "reverse")
            std::cout << "placeholder";
            //TODO: implement parse reverse Rules
        else if (rule_type == "shuffle")
            std::cout << "placeholder";
            //TODO: implement parse shuffle Rules
        else if (rule_type == "sort")
            std::cout << "placeholder";
            //TODO: implement parse sort Rules
        else if (rule_type == "deal")
            std::cout << "placeholder";
            //TODO: implement parse deal Rules
        else if (rule_type == "discard")
            std::cout << "placeholder";
            //TODO: implement parse discard Rules
        else if (rule_type == "add")
            std::cout << "placeholder";
            //TODO: implement parse add Rules
        else if (rule_type == "input-choice")
            std::cout << "placeholder";
            //TODO: implement parse input-choice Rules
        else if (rule_type == "input-text")
            std::cout << "placeholder";
            //TODO: implement parse input-text Rules
        else if (rule_type == "input-vote")
            std::cout << "placeholder";
            //TODO: implement parse input-vote Rules
        else if (rule_type == "message")
            std::cout << "placeholder";
            //TODO: implement parse message Rules
        else if (rule_type == "global-message")
            std::cout << "placeholder";
            //TODO: implement parse global-message Rules
        else if (rule_type == "scores")
            std::cout << "placeholder";
        //TODO: implement parse scores Rules*/
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

}

//entry point of rule parsing
gameSpecification::rule::RuleList
parseRule(const json& j){
    auto parsed_rules = gameSpecification::rule::RuleList();
    auto jsonRules = j["rules"].get<std::vector<json>>();
    for (auto rule : jsonRules) {
        parsed_rules.push_back(ruleSelector(rule, "foreach", ));
    }
    return parsed_rules;
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
