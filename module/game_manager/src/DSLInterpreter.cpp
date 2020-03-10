#include "DSLInterpreter.h"

#include <numeric>
#include <string_view>

namespace gameSpecification::rule {


void InterpretVisitor::visitImpl(const ForEach& forEach) {
    scope.push(&forEach);
    /*  
    for each expression in elemList:
      set loop variable to that expression by updating context
      for each rule:
        visit rule
  */
    auto elemList = boost::get<ExpList>(forEach.elemList);
    auto element = boost::get<std::string_view>(forEach.elem);
    for (const auto& exp : elemList.list) {
        context.map[element] = exp;
        for (const auto& rule : forEach.rules) {
            rule.accept(*this);
        }
    }
    if (!needUserInput) {
        scope.pop();
    }
}

std::string
interpolateString(const std::string_view str) {
    // parses strings like
    // "Round {round}. Choose your weapon!"
    // to "Round 1. Choose your weapon!"
}

Expression
evaluateExpression(const std::string_view str) {
    // parses string like
    // "configuration.Rounds.upfrom(1)"
    // into an actual expression
}

void InterpretVisitor::visitImpl(const GlobalMessage& globalMessage) {
    auto content = boost::get<std::string_view>(globalMessage.content);
    auto message = interpolateString(content);
}


// Question: difference b/w list and list expression in following example?

// { "rule": "foreach",
//   "list": <<list, list expression, or name of a list object>>,
//   "element": << name for list element object within the rules below >>
//   "rules": [
//     << Rules to execute on every element of the given list >>
//   ]
// }

}    // namespace gameSpecification::rule