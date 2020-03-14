#include "DSLInterpreter.h"

#include <numeric>
#include <string_view>
#include <iterator>
#include <algorithm>


namespace gameSpecification::rule {


void InterpretVisitor::visitImpl(ForEach& forEach) {
    /*  
    invariant: when this rule is called again each of its nested rules will have finished, 
    so it can increment its index to the next in the list. 
  */


    auto elemList = boost::get<ExpList>(forEach.elemList);
    auto element = boost::get<std::string_view>(forEach.elem);

    if(forEach.elemListIndex >= forEach.elemListSize) {
        forEach.finished = true;
        std::cout <<"finished for each loop\n";
        //remove the element variable from the context (as if leaving scope of for loop)
        context.map.erase(element);

        if(forEach.next != NULL){
            scope.push(forEach.next);
        }
        return;
    }
    else {
        forEach.nestedRulesInProgess = true;
    }


    auto exp = elemList.list[forEach.elemListIndex];
    context.map[element] = exp;

    if(forEach.next_nested != NULL) {
        scope.push(forEach.next_nested);
    }

    forEach.elemListIndex += 1;

}

void InterpretVisitor::visitImpl( Add& add) {

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
    
    std::cout << temp << ", " << value << "\n";

    add.finished = true;
    if(add.next != NULL) {
        scope.push(add.next);
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



void InterpretVisitor::visitImpl( GlobalMessage& globalMessage) {
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