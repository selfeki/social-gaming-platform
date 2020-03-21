#include "DSLInterpreter.h"
#include "arepa/game_spec/ExpressionPtr.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <string_view>
#include <cstdlib> 
#include <random>
#include <boost/variant.hpp>


namespace gameSpecification::rule {


std::string
interpolateString(const std::string_view str) {
    // parses strings like
    // "Round {round}. Choose your weapon!"
    // to "Round 1. Choose your weapon!"
    return "";
}

Expression
evaluateExpression(const std::string_view str) {
    // parses string like
    // "configuration.Rounds.upfrom(1)"
    // into an actual expression
    return 0;
}

// converts string players.weapon" to a vector {"players", "weapon"}
std::vector<std::string_view>
tokenizeDotNotation(std::string_view str) {
    std::vector<std::string_view> names;
    std::stringstream ss(static_cast<std::string>(str));
    std::string buffer;
    while (std::getline(ss, buffer, '.')) {
        names.push_back(buffer);
    }
    return names;
}

using typeVar = boost::variant<std::string_view, int, bool>;

// evaluates if '==', '<' and '>' Caution!! only pass bool, int, string Expressions 
bool 
evaluateEquality(const Expression exp1, const Expression exp2, const std::string_view comp){
    typeVar v1 ;
    typeVar v2 ;
    if (exp1.type() == typeid(int)){
        v1 = castExp<int>(exp1);
        v2 = castExp<int>(exp2);
    }
    else if(exp1.type() == typeid(bool)){
        v1 = castExp<bool>(exp1);
        v2 = castExp<bool>(exp2);
    }
    else{
         v1 = castExp<std::string_view>(exp1);
        v2 = castExp<std::string_view>(exp2);
    }
    
    if(comp == "==")
        return v1==v2;
    else if(comp == "<")
        return v1 < v2;
    else
        return v1 > v2;
}

// returns the list attribute after evaluating the 'element', 'Collect' condition
// {list name, attribute of list, 'contains'/'collect', contains attribute/ collect conditions}
// e.g players.elements.collect(player, player.weapon == weapon.beats)
std::vector<std::string_view>
evaluatelistExp(const Expression list){;
    auto listString = castExp<std::string_view>(list);
    auto openingBrackIndex = listString.find("(");
    auto beforeBracket = tokenizeDotNotation(listString.substr(0, openingBrackIndex-1));
    auto actualList = beforeBracket[0];
    auto attribute = beforeBracket[2];
    std::string_view contains_collect;
    if(attribute == "collect"){
        contains_collect = beforeBracket[2];
    }else{
        contains_collect = beforeBracket[3];
    }
    auto condition = listString.substr(openingBrackIndex+1, listString.length()-2);
    std::vector<std::string_view> result = {actualList, attribute, contains_collect, condition};
    return result;

}

// return a ExpList after evaluaing the 'contains' condition
ExpList
getContainsList(const ExpList list, Expression attribute, Expression toCompareTo){
    ExpList result;
    auto it = list.list.begin();
    while(it != list.list.end()){
        ExpMap map = castExp<ExpMap>(*it);
        auto playerVariableMap = (map.map.begin())->second;
        auto itr  = castExp<ExpMap>(playerVariableMap).map.find(
                            castExp<std::string_view>(attribute));
        if(itr != castExp<ExpMap>(playerVariableMap).map.end()){
            if(evaluateEquality(itr->second, toCompareTo, "==")){
                result.list.push_back(map);
            }
        }
    }
    return result;
}

void InterpretVisitor::visitImpl(InputChoice& rule) {
    // load gamestate with input request details
    //auto user = rule.targetUser;
    auto rawPrompt = castExp<std::string_view>(rule.prompt);
    auto prompt = interpolateString(rawPrompt);
    auto choices = castExp<ExpList>(rule.choiceList);
    auto resultStr = castExp<std::string_view>(rule.result);
    auto names = tokenizeDotNotation(resultStr);
    ExpressionPtr resultPtr(names);
    // set flag indicating need for user input
    // needUserInput = true;

    // todo: push next rule onto stack
    // todo: create InputRequest and to GameState
    // todo: implement pointers between sibling rules
}

void InterpretVisitor::visitImpl(ForEach& forEach) {
    /*  
    invariant: when this rule is called again each of its nested rules will have finished, 
    so it can increment its index to the next in the list. 
  */

    auto elemList = castExp<ExpList>(forEach.elemList);
    auto element = castExp<std::string_view>(forEach.elem);

    if (forEach.elemListIndex >= forEach.elemListSize) {
        forEach.finished = true;
        //remove the element variable from the context (as if leaving scope of for loop)

        // todo: refactor this (context switching should happen in GameInstance.updateState() ?)

        state.context.top().map.erase(element);

        return;
    } else {
        forEach.nestedRulesInProgess = true;
    }

    auto exp = elemList.list[forEach.elemListIndex];
    state.context.top().map[element] = exp;

    forEach.elemListIndex += 1;
}

void InterpretVisitor::visitImpl(Add& add) {


    // todo: refactor

    // auto to = castExp<std::string_view>(add.to);
    // auto to_tokens = parseDotNotation(to);

    // int value;

    // //is a literal
    // if (add.value.type() == typeid(int)) {
    //     value = castExp<int>(add.value);
    // }
    // //is a variable
    // else if (add.value.type() == typeid(std::string_view)) {
    //     auto value_tokens = parseDotNotation(castExp<std::string_view>(add.value));
    //     auto temp = this->getValueFromContextVariables(value_tokens);

    //     value = castExp<int>(temp);
    // }

    // //add the values
    // auto temp = castExp<int>(this->getValueFromContextVariables(to_tokens));
    // this->setValueOfContextVariables(to_tokens, Expression { temp + value });

    // std::cout << "TRACE inside add rule: " << temp << ", " << value << "\n";

    // add.finished = true;
}

void InterpretVisitor::visitImpl(GlobalMessage& globalMessage) {
    auto content = castExp<std::string_view>(globalMessage.content);
    auto message = interpolateString(content);
    // todo;
}

void InterpretVisitor::visitImpl(InputText& inputText) {
}

void InterpretVisitor::visitImpl(Reverse& reverse){
    // name of the list
    auto listName = castExp<std::string_view>(reverse.list);  
    // search for list in variabels
    auto currentState = this->getGameState();
    auto it = currentState.variables.map.find(listName);
    if(it == currentState.variables.map.end()){
        // todo:: Report Error - list not Found
        return;
    }
    std::reverse(castExp<ExpList>(it->second).list.begin(), 
                                    castExp<ExpList>(it->second).list.end());
    // update state
    this->setGameState(currentState);
}

void InterpretVisitor::visitImpl(Shuffle& shuffle){
    // name of list 
    auto listName = castExp<std::string_view>(shuffle.list);
    // search for list in state 
    auto currentState = this->getGameState();
    auto it = currentState.variables.map.find(listName);
    if(it == currentState.variables.map.end()){
        // todo :: return some error
        return;
    }
    // random number generator
    auto myrandom = std::default_random_engine {};
    std::shuffle(castExp<ExpList>(it->second).list.begin(), 
                                castExp<ExpList>(it->second).list.end(), myrandom);
    // // update state
    this->setGameState(currentState);
}

}    // namespace gameSpecification::rule
