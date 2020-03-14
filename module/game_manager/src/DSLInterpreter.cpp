#include "DSLInterpreter.h"

#include <numeric>
#include <string_view>

namespace gameSpecification::rule {


void InterpretVisitor::visitImpl(ForEach& forEach) {
    /*  
    invariant: when this rule is called again each of its nested rules will have finished, 
    so it can increment its index to the next in the list. 
  */

    auto elemList = boost::get<ExpList>(forEach.elemList);
    auto element = boost::get<std::string_view>(forEach.elem);

    auto exp = elemList.list[forEach.elemListIndex];
    context.map[element] = exp;
    scope.push(forEach.next_nested);

    forEach.elemListIndex += 1;

    if(forEach.elemListIndex >= forEach.elemListSize) {
        forEach.finished = true;
        scope.push(forEach.next);
    }
    else {
        forEach.nestedRulesInProgess = true;
    }

}

void InterpretVisitor::visitImpl(const Add& add) {
    scope.push(&add);

    auto to = boost::get<std::string_view>(add.to);
    int value;

    if(add.value.type() == typeid(int)) {
        value = boost::get<int>(add.value);
    }
    else if(add.value.type() == typeid(std::string_view)) {

        auto temp = context.map[boost::get<std::string_view>(add.value)];
        value = boost::get<int>(temp);
    } 
    else {
        //do nothing
    }
    auto temp = boost::get<int>(context.map[to]);

    //add the values
    context.map[to] = temp + value;
    
    scope.pop();
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