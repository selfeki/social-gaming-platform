#include "DSLInterpreter.h"
#include "gtest/gtest.h"
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
    ExpMap weaponsMap = {{
        {"rock", "scissors"},
        {"paper", "rock"},
        {"scissors", "paper"}
    }};
    ExpMap constantsMap = { { {"weapons", weaponsMap } } };
    std::string r="ramana", m="mehtab", a="anmol";
    ExpList winnenrsMap = {{r, m, a
                            }};
    ExpMap variabelsMap = { { {"winners", winnenrsMap } } };

    ExpMap winsMap = {{
                         {"Player1", 0}   }};
    ExpMap perPlayerMap = { { {"wins", winsMap } } };

    //empty perAudienceMap
    ExpMap perAudienceMap = { {  } };

    std::vector<ExpMap> context;
    RuleStateStack ruleStateStack;
    std::vector<TimerObject> timerList;

    // stores input requests to be delivered to users
    std::vector<InputRequest> inputRequests;
    std::vector<GameMessage>  messageQueue;

    GameState intialState = { constantsMap, variabelsMap, perPlayerMap, perAudienceMap, context, ruleStateStack, timerList, inputRequests, messageQueue};
    return intialState;
}

TEST(ListRulesTest, ReverseTest){
    // EXPECT_EQ(1, 1);
    auto intialState = getInitialState();
    std::string sw = "winners";
    rule::Reverse everse(sw);
    gameSpecification::rule::InterpretVisitor interpreter(intialState);
    interpreter.visitImpl(everse);
    auto newState = interpreter.getGameState();
    auto it = newState.variables.map.find("winners");
    if(it  == newState.variables.map.end()){
        printf("not found\n");
    }else{
        auto lister = castExp<ExpList>(it->second).list;
        auto li = lister.begin();
        while(li!=lister.end()){
            auto str = castExp<std::string>(*li);
            li++;
        }
        std::string r="ramana", m="mehtab", a="anmol";
        std::vector<Expression> expectedList = {a, m, r};

        for(int i=0 ; i < expectedList.size(); i++){
            auto str = castExp<std::string>(expectedList[i]);
            auto str2 = castExp<std::string>(lister[i]);
            EXPECT_EQ(str, str2) << "Vectors x and y differ at index " << i;
        }
    }
}


TEST(ListRulesTest, ShuffleTest){
    auto intialState = getInitialState();
    std::string sw = "winners";
    rule::Shuffle shuffle(sw);
    gameSpecification::rule::InterpretVisitor interpreter(intialState);
    interpreter.visitImpl(shuffle);
    auto newState = interpreter.getGameState();
    auto it = newState.variables.map.find("winners");
    if(it  == newState.variables.map.end()){
        printf("not found\n");
    }else{
        auto lister = castExp<ExpList>(it->second).list;
        auto li = lister.begin();
        std::vector<std::string> outputAsStr ;
        while(li!=lister.end()){
            auto str = castExp<std::string>(*li);
            outputAsStr.push_back(str);
            li++;
        }
        std::vector<std::string> expectedAsStr{"anmol","mehtab", "ramana"};
        ASSERT_NE(expectedAsStr, outputAsStr);

    }
}

TEST(ListRulesTest, ElementsTest){
     auto intialState = getInitialState();
    std::string w1 = "paper", w2 = "rock";
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
    std::string sw  ="players.elements.weapon" ;
    Expression exp = sw ;
    std::vector<std::string> expectedList = {w1, w2};
    auto outputList  = gameSpecification::rule::getEvaluatedList(
        gameSpecification::rule::evaluatelistExp(exp), intialState) ;

    for(int i = 0; i < outputList.list.size() ;  i++){
        auto outStr = castExp<std::string>(outputList.list[i]);
        std::cout<<i<< " "<< outStr<<std::endl;
        EXPECT_EQ(expectedList[i], outStr);
    }
                                
}

TEST(ListRulesTest, DiscardTest){
    auto intialState = getInitialState();
    std::string sw = "winners";
    Expression exp = sw;
    rule::Discard discard(exp, 1);
    gameSpecification::rule::InterpretVisitor interpreter(intialState);
    interpreter.visitImpl(discard);
    auto newState  = interpreter.getGameState();
    auto it = newState.variables.map.find(sw);
    if(it  == newState.variables.map.end()){
        printf("not found\n");
    }else{
        std::vector<std::string> expectedList = {"mehtab", "anmol"};
        auto outputList  = castExp<ExpList>(it->second).list;
        for(int i =0; i < expectedList.size(); i++){
            auto outStr = castExp<std::string>(outputList[i]);
            EXPECT_EQ(outStr, expectedList[i]);
        }
    } 
}


