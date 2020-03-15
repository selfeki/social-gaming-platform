#include "DSLInterpreter.h"

#include <numeric>
#include <string_view>
#include <iterator>
#include <algorithm>


namespace gameSpecification::rule {

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

//example, from "players.weapon" to a vector {"players", "weapon"}
//so that member variables can be accessed
std::vector<std::string_view> 
parseDotNotation(const std::string_view str) {
    std::vector<std::string_view> output;
    size_t first = 0;

    while(first < str.size()) {
        const auto second  = str.find_first_of(".", first);
        if(first != second) {
            output.emplace_back(str.substr(first, second - first));

            if(second == std::string_view::npos){
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

    for(auto it = tokens.begin(); it != tokens.end(); it++) {
        if(it == tokens.begin()) {
            exp = context.map[*it];
        }
        else {
            temp_map = boost::get<ExpMap>(exp);
            exp = temp_map.map[*it];
        }
    }
    return exp;
}

//given a vector of strings like {"player", "weapon", "strength"}, sets player.weapon.strength to the given expression
//necessary precondition: every variable is an ExpMap
void
InterpretVisitor::setValueOfContextVariables(std::vector<std::string_view> tokens, Expression value) {
    Expression* exp;
    ExpMap* temp_map = &context;

    for(auto it = tokens.begin(); it != tokens.end(); it++) {
        if(it == tokens.begin()) {
            exp = &(context.map[*it]);
        }
        else {
            temp_map = &(boost::get<ExpMap>(*exp));
            exp = &(temp_map->map[*it]);
        }
        if(std::next(it) == tokens.end()) {
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

    if(forEach.elemListIndex >= forEach.elemListSize) {
        forEach.finished = true;
        //remove the element variable from the context (as if leaving scope of for loop)
        context.map.erase(element);

        return;
    }
    else {
        forEach.nestedRulesInProgess = true;
    }

    auto exp = elemList.list[forEach.elemListIndex];
    context.map[element] = exp;

    forEach.elemListIndex += 1;
}

void InterpretVisitor::visitImpl( Add& add) {

    auto to = boost::get<std::string_view>(add.to);
    auto to_tokens = parseDotNotation(to);

    int value;

    //is a literal
    if(add.value.type() == typeid(int)) {
        value = boost::get<int>(add.value);
    }
    //is a variable
    else if(add.value.type() == typeid(std::string_view)) {
        auto value_tokens = parseDotNotation(boost::get<std::string_view>(add.value));
        auto temp = this->getValueFromContextVariables(value_tokens);

        value = boost::get<int>(temp);
    } 

    //add the values
    auto temp = boost::get<int>(this->getValueFromContextVariables(to_tokens));
    this->setValueOfContextVariables(to_tokens, Expression{temp + value});
    
    std::cout << "TRACE inside add rule: " << temp << ", " << value << "\n";

    add.finished = true;
}


void InterpretVisitor::visitImpl( GlobalMessage& globalMessage) {
    auto content = boost::get<std::string_view>(globalMessage.content);
    auto message = interpolateString(content);
}

void InterpretVisitor::visitImpl( InputText& inputText) {


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