#include "DSLInterpreter.h"

#include <numeric>
#include <string_view>
#include <iostream>

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

void 
InterpretVisitor::visitImpl(const ForEach& forEach) {
    ruleStack.push(&forEach);
    /*  
    for each expression in elemList:
      set loop variable to that expression by updating context
      for each rule:
        visit rule
  */
    //  add try-catch for runtime error
    auto elemList = boost::get<ExpList>(forEach.elemList);
    auto element = boost::get<std::string_view>(forEach.elem);
    for (const auto& exp : elemList.list) {
        auto& scope = state.context.top();
        scope.map[element] = exp;
        for (const auto& rule : forEach.rules) {
            state.enterScope();
            rule->accept(*this);
            state.exitScope();
        }
    }
    if (!needUserInput) {
        ruleStack.pop();
        state.exitScope();
    }
}

// { "rule": "input-choice",
//   "to": << a single player or audience member >>,
//   "prompt": << Message to send with request, as in "output" below  >>,
//   "choices": << list or name of a list to choose from >>
//   "result": << variable name in which to store the response >>

//   OPTIONAL
//   "timeout": << duration to wait for a response >>


void 
InterpretVisitor::visitImpl(const InputChoice& rule) {
    ruleStack.push(&rule);
    // load gamestate with input request details
    auto user = rule.targetUser;
    auto prompt = interpolateString(boost::get<std::string_view>(rule.prompt));
    auto choices = boost::get<ExpList>(rule.choiceList);
    // auto result = boost::get<
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


using namespace gameSpecification;

int main() {

    Expression expMap    = ExpMap({ { { "c", 1 } } });
    Expression nestedMap = ExpMap({ { { "b", expMap } } });
    ExpMap   nestederMap = ExpMap({ { { "a", nestedMap } } });
    
    // ExpMap& map = boost::polymorphic_strict_get<ExpMap>(expMap);
    // std::cout << &expMap << std::endl;

    ObjectPointer objPtr{{"a", "b", "c"}};

    auto refPtr = objPtr.dereferenceFromContext(nestederMap);

    if (!refPtr) {
        std::cout << "null";
    } else {
        // std::cout << "orig" << std::endl;
        // std::cout << &(expMap.map["a"]) << std::endl;
        // std::cout << "refPtr" << std::endl;
        // std::cout << refPtr.value() << std::endl;
        *(refPtr.value()) = 42;
        // std::cout << boost::get<int>(expMap.map["a"]);
        // Expression newExpMap = expMap;
        boost::apply_visitor(printExpVisitor(), static_cast<Expression>(nestederMap));
    }
    

    // refPtr.value() = 
    // ref 
    // std::cout << std::endl;
}