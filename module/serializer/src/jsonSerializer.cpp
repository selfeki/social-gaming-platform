#include "jsonSerializer.h"
#include "../../game_spec/include/Rule.h"

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
gameSpecification::rule::Rule
parseRuleReverse(const json& j){
    gameSpecification::Expression list = parseExpression(j.value("list", ""));
    gameSpecification::rule::Reverse reverseRule = {list};
    return reverseRule;
}

gameSpecification::rule::Rule
parseRuleShuffle(const json& j){
    gameSpecification::Expression list = parseExpression(j.value("list", ""));
    gameSpecification::rule::Shuffle shuffleRule = {list};
    return shuffleRule;
}

gameSpecification::rule::Rule
parseRuleAdd(const json& j){
    gameSpecification::Expression to = parseExpression(j.value("to", ""));
    gameSpecification::Expression value = parseExpression(j.value("value", 0));
    gameSpecification::rule::Add addRule= {to, value};
    return addRule;
}

//TODO: Finish off when
gameSpecification::rule::Rule
parseRuleWhen(const json& j){
        std::cout << j;
        //placeholder
        gameSpecification::rule::Extend extendRule = {"target", "list"};
        return extendRule;
}

gameSpecification::rule::Rule
parseRuleExtend(const json& j){
    gameSpecification::Expression target = parseExpression(j.value("target", ""));
    gameSpecification::Expression list = parseExpression(j.value("list", ""));
    gameSpecification::rule::Extend extendRule= {target, list};
    return extendRule;
}

gameSpecification::rule::Rule
parseRuleDiscard(const json& j){
    gameSpecification::Expression from = parseExpression(j.value("from", ""));
    gameSpecification::Expression count = parseExpression(j.value("count", ""));
    gameSpecification::rule::Discard discardRule = {from, count};
    return discardRule;
}

//todo: properly parse player/ player list. could be single value or list nlohmann isArray or something like that
gameSpecification::rule::Rule
parseRuleInputChoice(const json& j){
        //gameSpecification::Expression player = parseExpression(j.value("to", ""));
        std::vector<gameSpecification::rule::UserID> playerStub;

        gameSpecification::Expression prompt = parseExpression(j.value("prompt", ""));

        gameSpecification::Expression choices = parseExpression(j.value("choices", ""));

        gameSpecification::Expression result = parseExpression(j.value("result", ""));
        //TODO: timeout is num, but needs to be string
        //gameSpecification::Expression timeout = parseExpression(j.value("timeout", ""));

        gameSpecification::rule::InputChoice inputChoice = {playerStub, prompt, choices, result, "std::to_string(timeout)"};

        return inputChoice;
}

gameSpecification::rule::Rule
parseRuleGlobalMessage(const json& j){
    gameSpecification::Expression content = parseExpression(j.value("global-message", ""));
    gameSpecification::rule::GlobalMessage globMsg = {content};
    return globMsg;

}

gameSpecification::rule::Rule
parseRuleParallelFor(const json& j){
    gameSpecification::Expression list = parseExpression(j.value("list", ""));
    gameSpecification::Expression elem = parseExpression(j.value("element", ""));
    gameSpecification::rule::RuleList parsedSubRules = gameSpecification::rule::RuleList();
    auto subRules = j["rules"].get<std::vector<json>>();
    std::cout <<"HELLO";
    for(auto& rule : subRules){

        std::string ruleName = rule["rule"].get<std::string>();
        parsedSubRules.push_back(ruleSelector(rule, ruleName));
    }
    gameSpecification::rule::ParallelFor parallelFor = {list, elem, parsedSubRules};
    return parallelFor;
};

gameSpecification::rule::Rule
parseRuleForEach(const json& j){
    gameSpecification::Expression list = parseExpression(j.value("list", ""));
    gameSpecification::Expression elem = parseExpression(j.value("element", ""));
    //std::cout << boost::get<std::string>(list);

    gameSpecification::rule::RuleList parsedSubRules = gameSpecification::rule::RuleList();

    //std::cout << j["rules"].get<std::vector<json>>() << "AT END";

    auto subRules = j["rules"].get<std::vector<json>>();
    //std::cout << subRules;
    for(auto& rule : subRules){
        std::string ruleName = rule["rule"].get<std::string>();
        parsedSubRules.push_back(ruleSelector(rule, ruleName));
    }
    //TODO: call ruleSelector on each of the rules of THIS rule
    //and push into RulesList of THIS rule
    //ie: global-message, parallelfor, etc in the rock paper scissors example
    gameSpecification::rule::ForEach forEachRule = {list, elem, parsedSubRules};
    return forEachRule;
}

//think of this as the phone book of rules
gameSpecification::rule::Rule
ruleSelector(const json& j, const std::string& ruleName) {
        if (ruleName == "foreach")
            return parseRuleForEach(j);
        else if (ruleName == "loop")
            std::cout << "loop";
        else if (ruleName == "inparallel")
            std::cout << "inparallel";
        else if (ruleName == "parallelfor")
            return parseRuleParallelFor(j);
        else if (ruleName == "switch")
            std::cout << "switch";
        else if (ruleName == "when")
            return parseRuleWhen(j);
        else if (ruleName == "extend")
            return parseRuleExtend(j);
        else if (ruleName == "reverse")
            return parseRuleReverse(j);
        else if (ruleName == "shuffle")
            return parseRuleShuffle(j);
        else if (ruleName == "sort")
            std::cout << "sort";
        else if (ruleName == "deal")
            std::cout << "deal";
        else if (ruleName == "discard")
            return parseRuleDiscard(j);
        else if (ruleName == "add")
            return parseRuleAdd(j);
        else if (ruleName == "input-choice")
            return parseRuleInputChoice(j);
        else if (ruleName == "input-text")
            std::cout << "input-text";
        else if (ruleName == "input-vote")
            std::cout << "input-vote";
        else if (ruleName == "message")
            std::cout << "message";
        else if (ruleName == "global-message")
            return parseRuleGlobalMessage(j);
        else if (ruleName == "scores")
            std::cout << "scores";
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
    std::cout <<"parseRule";
    for (auto rule : jsonRules) {
        std::string ruleName = rule["rule"].get<std::string>();
        parsed_rules.push_back(ruleSelector(rule, ruleName));
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
