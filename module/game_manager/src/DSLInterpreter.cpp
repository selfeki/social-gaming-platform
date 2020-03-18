#include "DSLInterpreter.h"

#include "arepa/game_spec/ExpressionPtr.h"

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/variant/polymorphic_get.hpp>
#include <iostream>
#include <iterator>
#include <string_view>


namespace gameSpecification::rule {
// todo: change all boost::get to boost::polymorphic_strict_get
// create method for this and use it everywhere

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

std::vector<std::string_view>
getNameList(std::string_view str) {
    std::vector<std::string_view> names;
    std::stringstream ss(static_cast<std::string>(str));
    std::string buffer;
    while (std::getline(ss, buffer, '.')) {
        names.push_back(buffer);
    }
    return names;
}

/*
ForEach Rule:

for each expression in elemList:
    set loop variable to that expression by updating context
    for each rule:
    push rule onto ruleStack
    enter new scope to be used by rule
    visit rule
*/


void InterpretVisitor::visitImpl(InputChoice& rule) {
    // load gamestate with input request details
    //auto user = rule.targetUser;
    auto rawPrompt = boost::get<std::string_view>(rule.prompt);
    auto prompt = interpolateString(rawPrompt);
    auto choices = boost::get<ExpList>(rule.choiceList);
    auto resultStr = boost::get<std::string_view>(rule.result);
    auto names = getNameList(resultStr);
    ExpressionPtr resultPtr(names);
    // set flag indicating need for user input
    // needUserInput = true;

    // todo: push next rule onto stack
    // todo: create InputRequest and to GameState
    // todo: implement pointers between sibling rules
}

//example, from "players.weapon" to a vector {"players", "weapon"}
//so that member variables can be accessed
std::vector<std::string_view>
parseDotNotation(const std::string_view str) {
    std::vector<std::string_view> output;
    size_t first = 0;

    while (first < str.size()) {
        const auto second = str.find_first_of(".", first);
        if (first != second) {
            output.emplace_back(str.substr(first, second - first));

            if (second == std::string_view::npos) {
                break;
            }
        }
        first = second + 1;
    }

    return output;
}

//given a vector of strings like {"player", "weapon", "strength"}, returns player.weapon.strength from the context
//necessary precondition: every variable is an ExpMap
Expression
InterpretVisitor::getValueFromContextVariables(std::vector<std::string_view> tokens) {
    Expression exp;
    ExpMap temp_map;

    for (auto it = tokens.begin(); it != tokens.end(); it++) {
        if (it == tokens.begin()) {
            exp = context.map[*it];
        } else {
            temp_map = boost::get<ExpMap>(exp);
            exp = temp_map.map[*it];
        }
    }
    return exp;
}

//given a vector of strings like {"player", "weapon", "strength"}, sets player.weapon.strength to the given expression
//necessary precondition: every variable is an ExpMap
void InterpretVisitor::setValueOfContextVariables(std::vector<std::string_view> tokens, Expression value) {
    Expression* exp;
    ExpMap* temp_map = &context;

    for (auto it = tokens.begin(); it != tokens.end(); it++) {
        if (it == tokens.begin()) {
            exp = &(context.map[*it]);
        } else {
            temp_map = &(boost::get<ExpMap>(*exp));
            exp = &(temp_map->map[*it]);
        }
        if (std::next(it) == tokens.end()) {
            temp_map->map[*it] = value;
        }
    }
}


void InterpretVisitor::visitImpl(ForEach& forEach) {
    /*  
    invariant: when this rule is called again each of its nested rules will have finished, 
    so it can increment its index to the next in the list. 
  */

    auto elemList = boost::get<ExpList>(forEach.elemList);
    auto element = boost::get<std::string_view>(forEach.elem);

    if (forEach.elemListIndex >= forEach.elemListSize) {
        forEach.finished = true;
        //remove the element variable from the context (as if leaving scope of for loop)
        context.map.erase(element);

        return;
    } else {
        forEach.nestedRulesInProgess = true;
    }

    auto exp = elemList.list[forEach.elemListIndex];
    context.map[element] = exp;

    forEach.elemListIndex += 1;
}

void InterpretVisitor::visitImpl(Add& add) {

    auto to = boost::get<std::string_view>(add.to);
    auto to_tokens = parseDotNotation(to);

    int value;

    //is a literal
    if (add.value.type() == typeid(int)) {
        value = boost::get<int>(add.value);
    }
    //is a variable
    else if (add.value.type() == typeid(std::string_view)) {
        auto value_tokens = parseDotNotation(boost::get<std::string_view>(add.value));
        auto temp = this->getValueFromContextVariables(value_tokens);

        value = boost::get<int>(temp);
    }

    //add the values
    auto temp = boost::get<int>(this->getValueFromContextVariables(to_tokens));
    this->setValueOfContextVariables(to_tokens, Expression { temp + value });

    std::cout << "TRACE inside add rule: " << temp << ", " << value << "\n";

    add.finished = true;
}


void InterpretVisitor::visitImpl(GlobalMessage& globalMessage) {
    auto content = boost::get<std::string_view>(globalMessage.content);
    auto message = interpolateString(content);
    // todo;
}

void InterpretVisitor::visitImpl(InputText& inputText) {
}


}    // namespace gameSpecification::rule
