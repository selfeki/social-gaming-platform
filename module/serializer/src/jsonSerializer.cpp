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

#include <memory>

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
    for (const auto& [key, val] : j.at("setup").items()) {
        setup[key] = parseSetupValue(val);
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
        env[key] = exp;
    }
    return env;
}

gameSpecification::Expression
parseExpression(const json& j) {
    switch (j.type()) {
    case json::value_t::string:
      return std::string_view(j.get<std::string_view>());
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
    ExpMap expMap;
    for (const auto& [key, val] : j.items()) {
        Expression exp = parseExpression(val);
        expMap.map[key] = exp;
    }
    return expMap;
}

gameSpecification::Expression
parseExpList(const json& j) {
    ExpList expList;
    for (const auto& [_, val] : j.items()) {
        // todo: change to std::transform
        // todo: test expressions are serialized in right order
        Expression exp = parseExpression(val);
        expList.list.emplace_back(exp);
    }
    return expList;
}

std::unique_ptr<gameSpecification::rule::Rule>
parseRuleGlobalMessage(const json& j, gameSpecification::rule::Rule* parent){
    auto globMsg = std::make_unique<gameSpecification::rule::GlobalMessage>();
    globMsg->content = parseExpression(j.value("value", ""));
    globMsg->parent = parent;
    //boost::apply_visitor(printExpVisitor(), globMsg->content);
    return globMsg;

}

//todo: parse for targetUsers
std::unique_ptr<gameSpecification::rule::Rule>
parseRuleInputChoice(const json& j, gameSpecification::rule::Rule* parent){
    auto inputChoiceRule = std::make_unique<gameSpecification::rule::InputChoice>();
    //inputChoiceRule->targetUsers = parseExpression(j.value("to", ""));
    inputChoiceRule->prompt = parseExpression(j.value("prompt", ""));
    inputChoiceRule->choiceList = parseExpression(j.value("choices", ""));
    inputChoiceRule->result = parseExpression(j.value("result", ""));
    inputChoiceRule->timeout = parseExpression(j.value("timeout", 0));
    inputChoiceRule->parent = parent;

    //std::vector<gameSpecification::rule::UserID> playerStub;
    return inputChoiceRule;
    }
/*
//all other parseRule__RULENAME__ will be similar with different fields

//TODO: Finish off when
gameSpecification::rule::Rule
parseRuleWhen(const json& j){
        std::cout << j;
        //placeholder
        gameSpecification::rule::Extend extendRule = {"target", "list"};
        return extendRule;
}



*/
std::unique_ptr<gameSpecification::rule::Rule>
parseRuleParallelFor(const json& j, gameSpecification::rule::Rule* parent){
    auto parallelForRule = std::make_unique<gameSpecification::rule::ParallelFor>();
    parallelForRule->elemList = parseExpression(j.value("list", ""));
    parallelForRule->elem = parseExpression(j.value("element", ""));
    parallelForRule->parent = parent;
    parallelForRule->rules = gameSpecification::rule::RuleList();
    /*
    gameSpecification::rule::RuleList parsedSubRules = gameSpecification::rule::RuleList();
    auto subRules = j["rules"].get<std::vector<json>>();
    std::cout <<"HELLO";
    for(auto& rule : subRules){

        std::string ruleName = rule["rule"].get<std::string>();
        parsedSubRules.push_back(ruleSelector(rule, ruleName));
    }
    gameSpecification::rule::ParallelFor parallelFor = {list, elem, parsedSubRules};*/
    return parallelForRule;
};

std::unique_ptr<gameSpecification::rule::Rule>
parseRuleReverse(const json& j, gameSpecification::rule::Rule* parent){
    auto reverseRule = std::make_unique<gameSpecification::rule::Reverse>();
    reverseRule->list = parseExpression(j.value("list", ""));
    reverseRule->parent = parent;
    return reverseRule;
}

std::unique_ptr<gameSpecification::rule::Rule>
parseRuleInParallel(const json& j, gameSpecification::rule::Rule* parent){
    auto inParallelRule = std::make_unique<gameSpecification::rule::InParallel>();
    auto subRules = j["rules"].get<std::vector<json>>();
    for(auto& rule : subRules){
        std::string ruleName = rule["rule"].get<std::string>();
        //inParallelRule->rules.push_back(ruleSelector(rule, ruleName, inParallelRule.get()));
    }
}

std::unique_ptr<gameSpecification::rule::Rule>
parseRuleExtend(const json& j, gameSpecification::rule::Rule* parent){
    auto extendRule = std::make_unique<gameSpecification::rule::Extend>();
    extendRule->targetList = parseExpression(j.value("target", ""));
    extendRule->list = parseExpression(j.value("list", ""));
    extendRule->parent = parent;
    return extendRule;
}

std::unique_ptr<gameSpecification::rule::Rule>
parseRuleAdd(const json& j, gameSpecification::rule::Rule* parent){
    auto addRule = std::make_unique<gameSpecification::rule::Add>();
    addRule->to = parseExpList(j.value("to", ""));
    addRule->value = parseExpList(j.value("value", ""));
    addRule->parent = parent;
    return addRule;
}

std::unique_ptr<gameSpecification::rule::Rule>
parseRuleShuffle(const json& j, gameSpecification::rule::Rule* parent){
    auto shuffleRule = std::make_unique<gameSpecification::rule::Shuffle>();
    shuffleRule->list = parseExpression(j.value("list", ""));
    shuffleRule->parent = parent;
    return shuffleRule;
}

std::unique_ptr<gameSpecification::rule::Rule>
parseRuleDiscard(const json& j, gameSpecification::rule::Rule* parent){
        auto discardRule = std::make_unique<gameSpecification::rule::Discard>();
        discardRule->fromList = parseExpression(j.value("from", ""));
        discardRule->count = parseExpression(j.value("count", ""));
        discardRule->parent = parent;
        return discardRule;
    }

std::unique_ptr<gameSpecification::rule::Rule>
parseRuleMessage(const json& j, gameSpecification::rule::Rule* parent){
    auto messageRule = std::make_unique<gameSpecification::rule::Message>();
    messageRule->content = parseExpList(j.value("value", ""));
    messageRule->parent = parent;
    return messageRule;
}

std::unique_ptr<gameSpecification::rule::Rule>
parseRuleSort(const json& j, gameSpecification::rule::Rule* parent){
    auto sortRule = std::make_unique<gameSpecification::rule::Sort>();
    sortRule->list = parseExpression(j.value("list",""));
    sortRule->key = parseExpression(j.value("key",""));
    sortRule->parent = parent;
    return sortRule;
}

std::unique_ptr<gameSpecification::rule::Rule>
parseRuleDeal(const json& j, gameSpecification::rule::Rule* parent){
    auto dealRule = std::make_unique<gameSpecification::rule::Deal>();
    dealRule->fromList = parseExpression(j.value("from",""));
    dealRule->toList = parseExpression(j.value("to",""));
    dealRule->count = parseExpression(j.value("count",0));
    dealRule->parent = parent;
    return dealRule;
}

std::unique_ptr<gameSpecification::rule::Rule>
parseRuleScores(const json& j, gameSpecification::rule::Rule* parent){
    auto scoresRule = std::make_unique<gameSpecification::rule::Scores>();
    scoresRule->scoreAttribute = parseExpression(j.value("score",""));
    scoresRule->isAscending = parseExpression(j.value("ascending",""));
    scoresRule->parent = parent;
    return scoresRule;
}

std::unique_ptr<gameSpecification::rule::Rule>
parseRuleInputText(const json& j, gameSpecification::rule::Rule* parent){
    auto inputTextRule = std::make_unique<gameSpecification::rule::InputText>();
    //inputTextRule->targetUser = parseExpression(j.value("to", ""));
    inputTextRule->prompt = parseExpression(j.value("prompt", ""));
    inputTextRule->result = parseExpression(j.value("result", ""));
    inputTextRule->timeout = parseExpression(j.value("timeout",""));
    inputTextRule->parent = parent;
    return inputTextRule;
}

std::unique_ptr<gameSpecification::rule::Rule>
parseRuleInputVote(const json& j, gameSpecification::rule::Rule* parent){
    auto inputVoteRule = std::make_unique<gameSpecification::rule::InputVote>();
    //inputVoteRule->targetUsers = parseExpression(j.value("to",""));
    inputVoteRule->prompt = parseExpression(j.value("prompt", ""));
    inputVoteRule->choiceList = parseExpression(j.value("choices", ""));
    inputVoteRule->resultMap = parseExpression(j.value("result", ""));
    inputVoteRule->timeout = parseExpression(j.value("timeout",""));
    inputVoteRule->parent = parent;
    return inputVoteRule;
}

std::unique_ptr<gameSpecification::rule::Rule>
parseRuleForEach(const json& j, gameSpecification::rule::Rule* parent){
    auto forEachRule = std::make_unique<gameSpecification::rule::ForEach>();
    forEachRule->elemList = parseExpression(j.value("list", ""));
    forEachRule->elem = parseExpression(j.value("element", ""));
    forEachRule->parent = parent;
    forEachRule->rules = gameSpecification::rule::RuleList();

    auto subRules = j["rules"].get<std::vector<json>>();
    //std::cout << subRules;
    for(auto& rule : subRules){
        std::string ruleName = rule["rule"].get<std::string>();
        forEachRule->rules.push_back(ruleSelector(rule, ruleName, forEachRule.get()));
    }
    //TODO: call ruleSelector on each of the rules of THIS rule
    //and push into RulesList of THIS rule
    //ie: global-message, parallelfor, etc in the rock paper scissors example
    return forEachRule;
}



//think of this as the phone book of rules
//gameSpecification::rule::Rule&
std::unique_ptr<gameSpecification::rule::Rule>
ruleSelector(const json& j, const std::string& ruleName, gameSpecification::rule::Rule* parent) {

          //return gameSpecification::rule::ForEach(nullptr, "test","test", std::vector<gameSpecification::rule::Rule>);
            //return parseRuleForEach(j);
        if (ruleName == "foreach")
            return parseRuleForEach(j, parent);
        else if (ruleName == "parallelfor")
            return parseRuleParallelFor(j, parent);
        else if (ruleName == "global-message")
            return parseRuleGlobalMessage(j, parent);
        else if (ruleName == "input-choice")
            return parseRuleInputChoice(j, parent);
        else if (ruleName == "discard")
            return parseRuleDiscard(j, parent);
        else if (ruleName == "reverse")
            return parseRuleReverse(j, parent);
        else if (ruleName == "inparallel")
            return parseRuleInParallel(j, parent);
        else if (ruleName == "extend")
            return parseRuleExtend(j, parent);
        else if (ruleName == "add")
            return parseRuleAdd(j, parent);
        else if (ruleName == "shuffle")
            return parseRuleShuffle(j, parent);
        else if (ruleName == "message")
            return parseRuleMessage(j, parent);
        else if (ruleName == "sort")
            return parseRuleSort(j, parent);
        else if (ruleName == "deal")
            return parseRuleDeal(j, parent);
        else if (ruleName == "scores")
            return parseRuleScores(j, parent);
        else if (ruleName == "input-text")
            return parseRuleInputText(j, parent);
        else if (ruleName == "input-vote")
            return parseRuleInputVote(j, parent);
        /*
        else if (ruleName == "loop")
            std::cout << "loop";
        else if (ruleName == "switch")
            std::cout << "switch";
        else if (ruleName == "when")
            return parseRuleWhen(j);

*/
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
    //gameSpecification::rule::GlobalMessage rule = gameSpecification::rule::GlobalMessage();
    //rule.parent = nullptr;
    //std::cout <<"parseRule";
    for (auto rule : jsonRules) {
        std::string ruleName = rule["rule"].get<std::string>();
        parsed_rules.push_back(ruleSelector(rule, ruleName, nullptr));
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
