#include "DSLInterpreter.h"

#include "arepa/game_spec/ExpressionPtr.h"

#include <boost/algorithm/string.hpp>
#include <boost/variant/polymorphic_get.hpp>
#include <iostream>
#include <string_view>

namespace gameSpecification::rule {

// todo: change all boost::get to boost::polymorphic_strict_get

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

// todo: change to use Ryan's design
void InterpretVisitor::visitImpl(const ForEach& forEach) {
    auto elemList = boost::get<ExpList>(forEach.elemList);
    auto element = boost::get<std::string_view>(forEach.elem);
    for (const auto& exp : elemList.list) {
        auto& scope = state.context.top();
        scope.map[element] = exp;
        for (const auto& rulePtr : forEach.rules) {
            ruleStack.push(rulePtr.get());
            state.enterScope();

            rulePtr->accept(*this);
            if (needUserInput) {
                return;
            }
            state.exitScope();
            ruleStack.pop();
        }
    }
}

void InterpretVisitor::visitImpl(const InputChoice& rule) {
    // load gamestate with input request details
    //auto user = rule.targetUser;
    auto rawPrompt = boost::get<std::string_view>(rule.prompt);
    auto prompt = interpolateString(rawPrompt);
    auto choices = boost::get<ExpList>(rule.choiceList);
    auto resultStr = boost::get<std::string_view>(rule.result);
    auto names = getNameList(resultStr);
    ExpressionPtr resultPtr(names);
    // set flag indicating need for user input
    needUserInput = true;

    // todo: push next rule onto stack
    // todo: create InputRequest and to GameState
    // todo: implement pointers between sibling rules
}

void InterpretVisitor::visitImpl(const GlobalMessage& globalMessage) {
    auto content = boost::get<std::string_view>(globalMessage.content);
    auto message = interpolateString(content);
    // todo;
}


}    // namespace gameSpecification::rule
