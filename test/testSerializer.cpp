#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <nlohmann/json.hpp>
#include <typeinfo>
#include <stdio.h>
#include <string>
#include "arepa/serializer/jsonSerializer.h"
#include "arepa/server_config/Config.h"
#include "arepa/game_spec/MapWrapper.h"
#include "arepa/game_spec/Rule.h"


using json = nlohmann::json;

//Valid Stubs
json server_stub = R"({
    "port":4002,
    "html":"webchat.html",
    "games":[]
})"_json;

json config_stub = R"({
    "name":"rps",
    "player count": {
        "min":0,
        "max":4
    },
    "audience": false,
    "setup": {
        "Rounds": 10
    }
})"_json;

json game_stub = R"({
    "configuration": {
        "name": "Zen Game",
        "player count": {"min": 0, "max": 0},
        "audience": false,
        "setup": { }
    },
    "constants": {},
    "variables": {},
    "per-player": {},
    "per-audience": {},
    "rules": {}
})"_json;

//valid json
json Ptest_json = {
    {"configuration", {{"name","Zen Game"},
        {"player count", {
            {"min", 1},
            {"max", 2}
        }},
        {"audience", false},
        {"setup", {"set1","set2"}}
    }},
    {"constants", {"a","b","c"}},
    {"variables", {"a","b","c"}},
    {"per-player",{"pp1","pp2","pp3"}},
    {"per-audience",{"pa1"}},
    {"rules",{"rule1","rule2","rule3"}}
    };


TEST(jsonSerializerTest, parseServerConfig_confirmVariables) {
    serverConfig::Configuration stub;

    EXPECT_TRUE(typeid(jsonSerializer::parseServerConfig(server_stub)) == typeid(serverConfig::Configuration)); // check return type

    stub = jsonSerializer::parseServerConfig(server_stub);

    EXPECT_TRUE(server_stub["port"] != NULL);
    EXPECT_TRUE(typeid(stub.port) == typeid(int)); // check if integer
    EXPECT_TRUE(65535>=stub.port>0); // check if port is within TCP/IP port range

    EXPECT_TRUE(server_stub["html"] != NULL);
    EXPECT_TRUE(typeid(stub.htmlPath) == typeid(std::string)); // check if html value is string
    EXPECT_TRUE(stub.htmlPath.substr(stub.htmlPath.find(".") + 1) == "html"); // check if file is html
}

TEST(jsonSerializerTest, isValidServerConfig_validJson) {
    EXPECT_TRUE(jsonSerializer::isValidServerConfig(server_stub));
}

TEST(jsonSerializerTest, isValidServerConfig_missingPort) {
    json server_stub = R"({
        "html":"webchat.html",
        "games":[]
    })"_json;
    EXPECT_FALSE(jsonSerializer::isValidServerConfig(server_stub)); // should return false

}

TEST(jsonSerializerTest, isValidServerConfig_missingHtml) {
    json server_stub = R"({
        "port":4002,
        "games":[]
    })"_json;
    EXPECT_FALSE(jsonSerializer::isValidServerConfig(server_stub));

}

TEST(jsonSerializerTest, isValidServerConfig_missingGames) {
    json server_stub = R"({
        "port":4002,
        "html":"webchat.html"
    })"_json;
    EXPECT_FALSE(jsonSerializer::isValidServerConfig(server_stub));
}

TEST(jsonSerializerTest, isValidServerConfig_missingAll) {
    json server_stub = R"({
    })"_json;
    EXPECT_FALSE(jsonSerializer::isValidServerConfig(server_stub));
}

TEST(jsonSerializerTest, parseGameSpecification) {
    //todo
}

TEST(jsonSerializerTest, parseConfig) {
    gameSpecification::Configuration stub;

    EXPECT_TRUE(typeid(jsonSerializer::parseConfig(config_stub)) == typeid(gameSpecification::Configuration));
    
    stub = jsonSerializer::parseConfig(config_stub);

    EXPECT_TRUE(config_stub["name"] != NULL);
    EXPECT_TRUE(config_stub["player count"] != NULL);
    EXPECT_TRUE(config_stub["audience"] != NULL);
    EXPECT_TRUE(config_stub["setup"] != NULL);

    EXPECT_TRUE(typeid(stub.name) == typeid(std::string));
    EXPECT_TRUE(typeid(stub.count) == typeid(gameSpecification::PlayerCount));
    EXPECT_TRUE(typeid(stub.allowAudience) == typeid(bool));
    EXPECT_TRUE(typeid(stub.setup) == typeid(gameSpecification::Setup));

    EXPECT_TRUE(stub.count.min >= 0);
    EXPECT_TRUE(stub.count.max >= stub.count.min);
}

TEST(jsonSerializerTest, parseSetup_valid) {
    //TODO: Test to see if CustomSetup datatype works. 
    //      This will also test parseCustomSetup and parseDataKind also.
    json setup_stub = R"({
        "setup": {
            "value1":10,
            "value2":11,
            "value3":"hello world",
            "value4":true
        }
    })"_json;

    EXPECT_TRUE(typeid(jsonSerializer::parseSetup(setup_stub)) == typeid(gameSpecification::Setup));
    EXPECT_TRUE(typeid(setup_stub["setup"]["value1"].get<int>()) == typeid(int)); // just type checking of stub
    EXPECT_TRUE(typeid(setup_stub["setup"]["value2"].get<int>()) == typeid(int));
    EXPECT_TRUE(typeid(setup_stub["setup"]["value1"].get<std::string>()) == typeid(std::string)); // just type checking of stub
    EXPECT_TRUE(typeid(setup_stub["setup"]["value2"].get<bool>()) == typeid(bool));


    gameSpecification::Setup stub;
    stub = jsonSerializer::parseSetup(setup_stub);

    //DEBUG
    std::cout << "parseSetup_valid\n";
    std::cout << "value1: " << setup_stub["setup"]["value1"] << 
    ", returned: " << boost::get<int>(stub["value1"]) << "\n";
    std::cout << "value2: " << setup_stub["setup"]["value2"] << 
    ", returned: " << boost::get<int>(stub["value2"]) << "\n";
    std::cout << "value3: " << setup_stub["setup"]["value3"] << 
    ", returned: " << boost::get<std::string>(stub["value3"]) << "\n";
    std::cout << "value4: " << setup_stub["setup"]["value4"] << 
    ", returned: " << boost::get<bool>(stub["value4"]) << "\n";

    EXPECT_TRUE(boost::get<int>(stub["value1"]) == 10);
    EXPECT_TRUE(boost::get<int>(stub["value2"]) == 11);
    EXPECT_TRUE(boost::get<std::string>(stub["value3"]) == "hello world");
    EXPECT_TRUE(boost::get<bool>(stub["value4"]) == true);
}

TEST(jsonSerializerTest, parseSetup_noValue) {
    json setup_stub = R"({
        "setup": {}
    })"_json;

    EXPECT_TRUE(typeid(jsonSerializer::parseSetup(setup_stub)) == typeid(gameSpecification::Setup));

    gameSpecification::Setup stub;
    stub = jsonSerializer::parseSetup(setup_stub);

    EXPECT_TRUE(stub.empty());
}

TEST(jsonSerializerTest, parseSetup_inValid) {
    //TODO: test more invalid datatypes
    json setup_stub = R"({
        "setup": {
            "value1":1.1
        }
    })"_json;

    EXPECT_TRUE(typeid(jsonSerializer::parseSetup(setup_stub)) == typeid(gameSpecification::Setup));

    gameSpecification::Setup stub;
    stub = jsonSerializer::parseSetup(setup_stub);

    //TODO: test for correct error output. Currently no error handling
}

/*
ON HOLD: parseGameState causes seg. fault
TEST(jsonSerializerTest, parseGameState_valid) {
    //TODO: fill in per-audience field. I have no idea what goes in there.
    json game_stub = R"({
        "constants": {
            "weapons": [
                {"name":"Rock", "beats":"Scissors"},
                {"name":"Paper", "beats":"Rock"},
                {"name":"Scissors", "beats":"Paper"}
            ]
        },
        "variables": {
            "winners":[]
        },
        "per-player": {
            "wins":0
        },
        "per-audience": {}
    })"_json;

    gameSpecification::GameState stub;
    stub = jsonSerializer::parseGameState(game_stub);

    gameSpecification::printExpVisitor test;
    test(stub);
}
*/

TEST(jsonSerializerTest, parseGameState_emptyValues) {

    gameSpecification::GameState stub;
    stub = jsonSerializer::parseGameState(game_stub);

    EXPECT_TRUE(stub.constants.map.empty());
    EXPECT_TRUE(stub.variables.map.empty());
    EXPECT_TRUE(stub.perPlayer.map.empty());
    EXPECT_TRUE(stub.perAudience.map.empty());
}

TEST(jsonSerializerTest, parseExpression_string) {
    json exp_stub = R"({
        "value1" : "test"
    })"_json;

    gameSpecification::Expression stub;
    //casting to string view before assigning to expression
    stub = jsonSerializer::parseExpression(exp_stub["value1"]);

    EXPECT_TRUE(boost::get<std::string>(stub) =="test");
}

TEST(jsonSerializerTest, parseExpression_int) {
    json exp_stub = R"({
        "value1" : 1
    })"_json;

    gameSpecification::Expression stub;
    stub = jsonSerializer::parseExpression(exp_stub["value1"].get<int>());

    EXPECT_TRUE(boost::get<int>(stub) == 1);
}

TEST(jsonSerializerTest, parseExpression_bool) {
    json exp_stub = R"({
        "value1" : true
    })"_json;

    gameSpecification::Expression stub;
    stub = jsonSerializer::parseExpression(exp_stub["value1"]);

    EXPECT_TRUE(boost::get<bool>(stub) == true);
}

TEST(jsonSerializerTest, parseExpression_expmap) {
    json exp_stub = R"({
        "value1" : {"name":"test", "value":"test"}
    })"_json;

    gameSpecification::Expression stub;
    stub = jsonSerializer::parseExpression(exp_stub["value1"]);
    
    EXPECT_TRUE(boost::get<std::string>(boost::get<gameSpecification::ExpMap>(stub).map["name"]) == "test");
}

/*
TODO: parseExpList needs fixing.
TEST(jsonSerializerTest, parseExpression_explist) {
    json exp_stub = R"({
        "value1" : [1,2,3,4]
    })"_json;

    gameSpecification::Expression stub;
    stub = jsonSerializer::parseExpression(exp_stub["value1"]);
    
// // }
// // */

// // //TODO: create testcases for all rules

// // /*
// // ONHOLD: Can't find rulevisitor's implementation
// // TEST(jsonSerializerTest, parseRuleGlobalMessage) {
// //     json rule_stub = R"(
// //         { "rule": "global-message",
// //           "value": "Round {round}. Choose your weapon!"
// //         }
// //     )"_json;

// //     gameSpecification::rule::RuleVisitor visitor;
// //     gameSpecification::rule::Rule* parent_stub;
// //     std::unique_ptr<gameSpecification::rule::Rule> stub;
// //     stub = jsonSerializer::parseRuleGlobalMessage(rule_stub, parent_stub);

// //     EXPECT_TRUE(boost::get<std::string_view>(stub->content) == "Round {round}. Choose your weapon!");

// // }
// // */


TEST(jsonSerializerTest, hasNoExtraFields) {
    //invalid json - additional field in player count
    json Ftest1_json = {
        {"configuration", {{"name","Zen Game"},
            {"player count", {
                {"min", 1},
                {"max", 2},
                {"additionalField", 3}
            }},
            {"audience", "false value"},
            {"setup", {"set1","set2"}}
        }},
        {"constants", {"a","b","c"}},
        {"variables", {"a","b","c"}},
        {"per-player",{"pp1","pp2","pp3"}},
        {"per-audience",{"pa1"}},
        {"rules",{"rule1","rule2","rule3"}}
    };
    //invalid json - additional field in main fields
    json Ftest2_json = {
        {"configuration", {{"name","Zen Game"},
            {"player count", {
                {"min", 1},
                {"max", 2}
            }},
            {"audience", "false value"},
            {"setup", {"set1","set2"}}
        }},
        {"constants", {"a","b","c"}},
        {"variables", {"a","b","c"}},
        {"additionalField", {"badField1","badField2"}},
        {"per-player",{"pp1","pp2","pp3"}},
        {"per-audience",{"pa1"}},
        {"rules",{"rule1","rule2","rule3"}}
    };
    EXPECT_TRUE(jsonSerializer::hasNoExtraFields(game_stub));
    EXPECT_FALSE(jsonSerializer::hasNoExtraFields(Ftest1_json));
    EXPECT_FALSE(jsonSerializer::hasNoExtraFields(Ftest2_json));
}

TEST(jsonSerializerTest, hasAllRequiredFields) {
    //invalid json - missing "constants"
    json Ftest1_json = {
        {"configuration", {{"name","Zen Game"},
            {"player count", {
                {"min", 1},
                {"max", 2}
            }},
            {"audience", false},
            {"setup", {"set1","set2"}}
        }},
        {"missing", {"a","b","c"}},
        {"variables", {"a","b","c"}},
        {"per-player",{"pp1","pp2","pp3"}},
        {"per-audience",{"pa1"}},
        {"rules",{"rule1","rule2","rule3"}}
    };
    //invalid json - missing "player count"
    json Ftest2_json = {
        {"configuration", {{"name","Zen Game"},
            {"missing", {
                {"min", 1},
                {"max", 2}
            }},
            {"audience", false},
            {"setup", {"set1","set2"}}
        }},
        {"constants", {"a","b","c"}},
        {"variables", {"a","b","c"}},
        {"per-player",{"pp1","pp2","pp3"}},
        {"per-audience",{"pa1"}},
        {"rules",{"rule1","rule2","rule3"}}
    };
    EXPECT_TRUE(jsonSerializer::hasAllRequiredFields(game_stub));
    EXPECT_FALSE(jsonSerializer::hasAllRequiredFields(Ftest1_json));
    EXPECT_FALSE(jsonSerializer::hasAllRequiredFields(Ftest2_json));
}

TEST(jsonSerializerTest, validFieldValues) {
    //invalid json - wrong type in "max"
    json Ftest1_json = {
        {"configuration", {{"name","Zen Game"},
            {"player count", {
                {"min", 1},
                {"max", "2"}
            }},
            {"audience", "false value"},
            {"setup", {"set1","set2"}}
        }},
        {"constants", {"a","b","c"}},
        {"variables", {"a","b","c"}},
        {"per-player",{"pp1","pp2","pp3"}},
        {"per-audience",{"pa1"}},
        {"rules",{"rule1","rule2","rule3"}}
    };
    //invalid json - wrong type in "variables"
    json Ftest2_json = {
        {"configuration", {{"name","Zen Game"},
            {"player count", {
                {"min", 1},
                {"max", 2}
            }},
            {"audience", "false value"},
            {"setup", {"set1","set2"}}
        }},
        {"constants", {"a","b","c"}},
        {"variables", 1},
        {"per-player",{"pp1","pp2","pp3"}},
        {"per-audience",{"pa1"}},
        {"rules",{"rule1","rule2","rule3"}}
    };
    EXPECT_TRUE(jsonSerializer::validFieldValues(Ptest_json));
    EXPECT_FALSE(jsonSerializer::validFieldValues(Ftest1_json));
    EXPECT_FALSE(jsonSerializer::validFieldValues(Ftest2_json));
}

TEST(jsonSerializerTest, isValidGameSpec) {
    //invalid json - missing "player count"
    json Ftest_json = {
        {"configuration", {{"name","Zen Game"},
            {"missing", {
                {"min", 1},
                {"max", 2}
            }},
            {"audience", false},
            {"setup", {"set1","set2"}}
        }},
        {"constants", {"a","b","c"}},
        {"variables", {"a","b","c"}},
        {"per-player",{"pp1","pp2","pp3"}},
        {"per-audience",{"pa1"}},
        {"rules",{"rule1","rule2","rule3"}}
    };
    EXPECT_TRUE(jsonSerializer::isValidGameSpec(Ptest_json));
    EXPECT_FALSE(jsonSerializer::isValidGameSpec(Ftest_json));
}