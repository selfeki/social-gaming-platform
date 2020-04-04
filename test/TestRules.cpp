#include "DSLInterpreter.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <string_view>
#include <cstdlib> 
#include <random>
#include <boost/variant.hpp>

#include <sstream>

GameState getInitialState(){
    GamePlayer player1 = {"Player1", {}};
    GameAudience audience1 = {"audience1", {}};
    ExpMap weaponsMap = {{
        {"rock", "scissors"},
        {"paper", "rock"},
        {"scissors", "paper"}
    }};
    ExpMap constantsMap = { { {"weapons", weaponsMap } } };
    std::string_view r="ramana", m="mehtab", a="anmol";
    ExpList winnenrsMap = {{r, m, a
                            }};
    ExpMap variabelsMap = { { {"winners", winnenrsMap } } };

    ExpMap winsMap = {{
                         {"Player1", 0}   }};
    ExpMap perPlayerMap = { { {"wins", winsMap } } };

    //empty perAudienceMap
    ExpMap perAudienceMap = { {  } };

    std::stack<Environment> context;

    // stores input requests to be delivered to users
    std::vector<InputRequest> inputRequests;

    GameState intialState = { constantsMap, variabelsMap, perPlayerMap, perAudienceMap, context, inputRequests, {player1}, {audience1}};
    return intialState;
}

// TEST(ListRulesTest, ReverseTest){
//     // EXPECT_EQ(1, 1);
//     auto intialState = getInitialState();
//     std::string_view sw = "winners";
//     rule::Reverse everse(sw);
//     gameSpecification::rule::InterpretVisitor interpreter(intialState);
//     interpreter.visitImpl(everse);
//     auto newState = interpreter.getGameState();
//     auto it = newState.variables.map.find("winners");
//     if(it  == newState.variables.map.end()){
//         printf("not found\n");
//     }else{
//         auto lister = castExp<ExpList>(it->second).list;
//         auto li = lister.begin();
//         while(li!=lister.end()){
//             auto str = castExp<std::string_view>(*li);
//             printf(" val -> %s \n", str);
//             li++;
//         }
//         std::string_view r="ramana", m="mehtab", a="anmol";
//         std::vector<Expression> expectedList = {a, m, r};

//         for(int i=0 ; i < expectedList.size(); i++){
//             auto str = castExp<std::string_view>(expectedList[i]);
//             auto str2 = castExp<std::string_view>(lister[i]);
//             EXPECT_EQ(str, str2) << "Vectors x and y differ at index " << i;
//         }
//     }
// }


// TEST(ListRulesTest, ShuffleTest){
//     auto intialState = getInitialState();
//     std::string_view sw = "winners";
//     rule::Shuffle shuffle(sw);
//     gameSpecification::rule::InterpretVisitor interpreter(intialState);
//     interpreter.visitImpl(shuffle);
//     auto newState = interpreter.getGameState();
//     auto it = newState.variables.map.find("winners");
//     if(it  == newState.variables.map.end()){
//         printf("not found\n");
//     }else{
//         auto lister = castExp<ExpList>(it->second).list;
//         auto li = lister.begin();
//         std::vector<std::string_view> outputAsStr ;
//         while(li!=lister.end()){
//             auto str = castExp<std::string_view>(*li);
//             outputAsStr.push_back(str);
//             printf(" val -> %s \n", str);
//             li++;
//         }
//         std::vector<std::string_view> expectedAsStr{"anmol","mehtab", "ramana"};
//         ASSERT_NE(expectedAsStr, outputAsStr);

//     }
// }

TEST(ListRulesTest, DiscardTest){
    auto intialState = getInitialState();
    std::string_view w1 = "paper", w2 = "rock";
    ExpMap map1 = {{
        {"weapon", w1}
    }};
    ExpMap map2 = {{
            {"weapon", w2}
    }};    
    ExpMap player1Map = {{
        {"player1", map1}
    }};
    ExpMap player2Map = {{
        {"player2", map2}
    }};
    ExpList players{{player1Map, player2Map}};
    intialState.variables.map.insert({"players", players});
    std::string_view sw = "players.elements.weapon";
    rule::Discard discard(sw, 1);
    auto newState = intialState;
    auto it = newState.variables.map.find("players");

        gameSpecification::rule::InterpretVisitor interpreter(intialState);
    interpreter.visitImpl(discard);

    if(it  == newState.variables.map.end()){
        printf("not found\n");
    }else{
        auto lister = castExp<ExpList>(it->second).list[0];
        auto playerm = castExp<ExpMap>(lister).map;
        auto li = playerm.begin();
        auto actualMap = castExp<ExpMap>(li->second).map;
        auto realItr = actualMap.begin();
        while(realItr != actualMap.end()){
            auto ma = castExp<std::string_view>(realItr->second);
            // auto ra  = castExp<std::string_view>(ma->first);
            std::cout<<ma<<std::endl;
            realItr++;
        }

    } 
}


