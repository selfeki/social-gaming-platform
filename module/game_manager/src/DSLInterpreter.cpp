#include "DSLInterpreter.h"
#include "arepa/game_spec/ExpressionPtr.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string_view>


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

// converts string "players.weapon" to a vector {"players", "weapon"}
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


}    // namespace gameSpecification::rule
