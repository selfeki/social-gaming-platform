#pragma once

#include "arepa/game_spec/GameSpecification.h"
#include "arepa/server_config/Config.h"
#include "arepa/game_spec/Expression.h"
#include "arepa/game_spec/Rule.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <vector>
#include <optional>

namespace jsonSerializer {


using json = nlohmann::json;

serverConfig::Configuration
parseServerConfig(const json&);

gameSpecification::Specification
parseGameSpecification(const json&);

gameSpecification::Configuration
parseConfig(const json&);

gameSpecification::Setup
parseSetup(const json&);

gameSpecification::SetupValue
parseSetupValue(const json&);

gameSpecification::CustomSetup
parseCustomSetup(const json&);

gameSpecification::DataKind
parseDataKind(const json&);

gameSpecification::GameState
parseGameState(const json&);

gameSpecification::Environment
parseEnvironment(const json&);

gameSpecification::Expression
parseExpression(const json&);

gameSpecification::Expression
parseExpMap(const json&);

gameSpecification::Expression
parseExpList(const json&);

//gameSpecification::rule::Rule
//gameSpecification::rule::Rule&
std::unique_ptr<gameSpecification::rule::Rule>
ruleSelector(const json&, const std::string&, gameSpecification::rule::Rule*);

gameSpecification::rule::RuleList
parseRule(const json&);


//gameSpecification::rule::Rule
std::unique_ptr<gameSpecification::rule::Rule>
parseRuleGlobalMessage(const json&);

std::unique_ptr<gameSpecification::rule::Rule>
parseRuleForEach(const json& ,gameSpecification::rule::Rule* parent);


std::unique_ptr<gameSpecification::rule::Rule>
parseRuleParallelFor(const json& ,gameSpecification::rule::Rule* parent);

/*
gameSpecification::rule::Rule
parseRuleInputChoice(const json& );

gameSpecification::rule::Rule
parseRuleDiscard(const json& );

gameSpecification::rule::Rule
parseRuleExtend(const json&);

gameSpecification::rule::Rule
parseRuleWhen(const json&);

gameSpecification::rule::Rule
parseRuleAdd(const json&);

gameSpecification::rule::Rule
parseRuleReverse(const json&);

gameSpecification::rule::Rule
parseRuleShuffle(const json&);
 */

bool isValidServerConfig(const json&);

bool isValidGameSpec(const json&);

bool hasAllRequiredFields(const json&);

bool hasNoExtraFields(const json&);



// enum FIELDS {
//     CONF,
//     NAME,
//     PLYRCOUNT,
//     AUDI,
//     SETUP,
//     CONST,
//     VAR,
//     PPLYR,
//     PAUDI,
//     RULES
// };

// const std::unordered_map<FIELDS, std::string> enum_to_str({
//     {CONF,      "/configuration"},
//     {NAME,      "/configuration/name"},
//     {PLYRCOUNT, "/configuration/player count"},
//     {AUDI,      "/configuration/audience"},
//     {SETUP,     "/configuration/setup"},
//     {CONST,     "/constants"},
//     {VAR,       "/variables"},
//     {PPLYR,     "/per-player"},
//     {PAUDI,     "/per-audience"},
//     {RULES,     "/rules"}
// });


}    // namespace jsonSerializer
