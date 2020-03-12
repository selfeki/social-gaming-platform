#pragma once

#include "arepa/game_spec/GameSpecification.h"
#include "arepa/server_config/Config.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <vector>


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

gameSpecification::rule::RuleList
parseRules(const json&);

bool isValidServerConfig(const json&);

bool isValidGameSpec(const json&);

bool hasAllRequiredFields(const json&);

bool hasNoExtraFields(const json&);

bool validFieldValues(const json& j);

//json configuration requirements:
//used by hasAllRequiredFields(), hasNoExtraFields(), and validFieldValues() to validate json fields
const std::map<std::string,json::value_t> FIELDS = {
												{"configuration", json::value_t::object},
												{"constants", json::value_t::array},
												{"variables", json::value_t::array},
												{"per-player", json::value_t::array},
												{"per-audience", json::value_t::array},
												{"rules", json::value_t::array}
												};
const std::map<std::string,json::value_t> FIELDS_configuration = {
												{"name", json::value_t::string},
												{"player count", json::value_t::object},
												{"audience", json::value_t::boolean},
												{"setup", json::value_t::array}
												};
const std::map<std::string,json::value_t> FIELDS_playerCount = {
												{"min", json::value_t::number_integer},
												{"max", json::value_t::number_integer},
												};

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
