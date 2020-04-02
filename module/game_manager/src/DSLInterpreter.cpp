#include "DSLInterpreter.h"
#include "arepa/game_spec/ExpressionPtr.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string_view>


namespace gameSpecification::rule {


int
countUnclosedParenthesis(std::string str) {
    int count = 0;
    for (const auto& c : str) {
        if (c == '(') { count++; }
        if (c == ')') { count--; }
    }
    return count;
}

/*  
    converts "players.elements.weapon.contains(weapon.name)"
    into { "players", "elements", "weapon", "contains(weapon.name)" }
*/
std::vector<std::string>
InterpretVisitor::tokenizeDotNotation(std::string_view str) {
    std::vector<std::string> names;
    std::stringstream ss(static_cast<std::string>(str));
    std::string buffer;
    std::string temp = "";
    int unclosedParCount = 0;
    while (std::getline(ss, buffer, '.')) {
        unclosedParCount += countUnclosedParenthesis(buffer);
        if (unclosedParCount == 0) {
            buffer.insert(0, temp);
            names.push_back(buffer);
            temp.erase();
        } else {
            temp.append(buffer);
            temp.append(".");
        }
    }
    if (!temp.empty()) { names.push_back(temp); }
    return names;
}

std::optional<Expression>
InterpretVisitor::lookupLocalScope(const std::vector<std::string>& indices) {
    ExpressionPtr expPtr(indices);
    auto& context = getGameState().context;
    auto found = std::find_if(context.rbegin(), context.rend(),
        [&expPtr](auto& expMap) { return expPtr.getPtr(expMap); }
    );
    if (found == context.rend()) {
        return { };
    }
    auto getPtrRes = expPtr.getPtr(*found);
    auto resPtr    = getPtrRes.value();
    return *resPtr;
}

std::optional<Expression>
InterpretVisitor::lookupGlobalScope(const std::vector<std::string>& indices) {
    ExpressionPtr expPtr(indices);
    auto getPtrVars = expPtr.getPtr(state.variables);
    if (getPtrVars) {
        auto resPtr = getPtrVars.value();
        return *resPtr;
    }
    auto getPtrConsts = expPtr.getPtr(state.constants);
    if (getPtrConsts) {
        auto resPtr = getPtrConsts.value();
        return *resPtr;
    }
    return { };
}

std::optional<Expression>
InterpretVisitor::lookupWithIndices(const std::vector<std::string>& indices) {
    auto expLocalScope = lookupLocalScope(indices);
    if (expLocalScope) {
        return expLocalScope;
    }
    return lookupGlobalScope(indices);
}

std::optional<Expression>
InterpretVisitor::lookupWithString(std::string_view name) {
    auto indices = tokenizeDotNotation(name);
    return lookupWithIndices(indices);
}

/*
    parses strings like
    "Round {round}. Choose your weapon!"
    into  "Round 1. Choose your weapon!"
*/
std::string
InterpretVisitor::interpolateString(std::string_view str) {
    return "";
    // todo
}

const auto ATTRIBUTES = { "upfrom", "size", "elements", "contains", "collect" };

bool
isAttribute(const std::string_view str) {
    for (const auto& attr : ATTRIBUTES) {
        if (str.find(attr) != std::string::npos) {
            return true;
        } 
    }
    return false;
}

Expression
applyAttribute(const Expression& exp, std::string_view) {
    // todo
    return 0;
}

 /* 
    Parses string like "configuration.Rounds.upfrom(1)" into an actual expression

    Some useful grammar productions:
            name -> a  string
            listAttribute -> size | elements.name | elements.contains | elements.collect
                (note: elements.name only applies to list of maps)
            intAttribute  -> upfrom(1)
            attribute -> listAttribute | intAttribute
            names -> name(.name)*(.attribute)*

    special names
        int: upfrom(int) -> list of ints
        list: size -> int, contains(expression) -> bool, collect(paramName, expression evaluating to bool)
        list of maps: elements."name" -> list of Ty(value associated with "name")
*/
// note: must handle "a == b" as a possible expression
Expression
InterpretVisitor::evaluateExpression(const Expression& exp) {
    if (exp.type() != typeid(std::string)) {
        // the value is a literal, nothing to evaluate
        return exp;
    }
    auto str    = castExp<std::string>(exp);
    auto tokens = tokenizeDotNotation(str);
    // separate names that refer to expressions from attributes, if any
    auto namesEndIter = tokens.begin();
    while (namesEndIter != tokens.end()) {
        auto token = *namesEndIter;
        if (isAttribute(token)) { break; }
        namesEndIter++;
    }
    std::vector<std::string> names;
    std::vector<std::string> attributes;
    std::move(tokens.begin(), namesEndIter, std::back_inserter(names));
    std::move(namesEndIter, tokens.end(), std::back_inserter(attributes));
    // evaluate names
    auto lookupRes = lookupWithIndices(names);
    if (!lookupRes) {
        throw std::runtime_error("invalid object name in evaluateExpression: " + std::string(str));
    }
    auto resExp = lookupRes.value();
    // apply attributes
    for (const auto& attr : attributes) {
        resExp = applyAttribute(resExp, attr);
    }
    return resExp;
}

void 
InterpretVisitor::visitImpl(InputChoice& rule) {
    // load gamestate with input request details
    //auto user = rule.targetUser;
    auto rawPrompt = castExp<std::string>(rule.prompt);
    auto prompt = interpolateString(rawPrompt);
    auto choices = castExp<ExpList>(rule.choiceList);
    auto resultStr = castExp<std::string>(rule.result);
    auto names = tokenizeDotNotation(resultStr);
    ExpressionPtr resultPtr(names);
    // set flag indicating need for user input
    // needUserInput = true;

}

void 
InterpretVisitor::visitImpl(ForEach& forEach) {
    /*  
    invariant: when this rule is called again each of its nested rules will have finished, 
    so it can increment its index to the next in the list. 
  */

    auto elemList = castExp<ExpList>(forEach.elemList);
    auto element = castExp<std::string>(forEach.elem);
    
    //if this rule is being called, the rule_state object that belongs to this rule
    //should be at the top of the interpreters stack, so get it
    //TODO: maybe this isnt a good way to get the rule_state associated with this rule. Should be passed in to .accept() with the rule maybe?
    auto rule_state = &(state.ruleStateStack.top());

    if (rule_state->elemListIndex >= forEach.elemListSize) {
        rule_state->finished = true;
        //remove the element variable from the context (as if leaving scope of for loop)

        // todo: refactor this (context switching should happen in GameInstance.updateState() ?)

        state.context.back().map.erase(element);

        return;
    } else {
        rule_state->nestedRulesInProgess = true;
    }

    auto exp = elemList.list[rule_state->elemListIndex];
    state.context.back().map[element] = exp;

    rule_state->elemListIndex += 1;
}

void 
InterpretVisitor::visitImpl(Add& add) {

    auto rule_state = &(state.ruleStateStack.top());

    auto toExp       = castExp<std::string>(add.to);
    auto toExpTokens = tokenizeDotNotation(toExp);
    auto toExpPtr    = ExpressionPtr(toExpTokens);
    auto getPtrRes   = toExpPtr.getPtr(state.variables);
    if (!getPtrRes) {
        throw std::runtime_error("invalid \"to\" object name in Add rule");
    }
    auto ptr         = getPtrRes.value();
    auto to          = castExp<int>(*ptr);
    auto valueExp    = evaluateExpression(add.value);
    auto value       = castExp<int>(valueExp);
    auto sum = to + value;
    // assuming rules can only write to memory within the Variables environment
    toExpPtr.store(sum, state.variables);
    rule_state->finished = true;
}

void 
InterpretVisitor::visitImpl(GlobalMessage& globalMessage) {
    auto content = castExp<std::string>(globalMessage.content);
    auto message = interpolateString(content);
    state.enqueueGlobalMessage(message);
}

void
InterpretVisitor::visitImpl(Timer& timer) {

    /*
        for every loop in GameInstance::updateState() , 
            check if timer object (not this rule, but an object in the game state) is finished. If it is mode 'at most' or 'exact', 
            and the timer has completed, pop the nested timer rules and carry on to the next rule

            if the timer object is of type 'track', set the flag
    */
    auto rule_state = &(state.ruleStateStack.top());


   if(!rule_state->nestedRulesInProgess) { //this rule is being called for the first time
        rule_state->nestedRulesInProgess = true;

        //create a timer object 
        auto duration = castExp<int>(timer.duration);
        state.timerList.emplace_back(TimerObject{static_cast<double>(duration), &timer, timer.mode});
   } 
   else { //this rule's nested rules have completed
        rule_state->nestedRulesInProgess = false;

        //find the timer object belonging to this rule
        auto my_timer = std::find_if(state.timerList.begin(), 
                                state.timerList.end(), 
                                [&timer] (TimerObject& t) {
                                    return t.owner == &timer;
                                });

        if((*my_timer).mode == TimerMode::EXACT) {
            if((*my_timer).isTimeOut()){
                rule_state->finished = true;
            } 
        } 
        else if ((*my_timer).mode == TimerMode::AT_MOST){
            rule_state->finished = true;
        } 
        else { //TimerMode == TRACK
            //GameInstance::updateState() may have already set the flag, but set it anyway
            //assuming the flag is a variable
            auto flagExp       = castExp<std::string>(*(timer.flag)); 
            auto flagExpTokens = tokenizeDotNotation(flagExp);
            auto flagExpPtr    = ExpressionPtr(flagExpTokens);
            flagExpPtr.store(Expression{true}, state.variables);
        }
   }
}



void 
InterpretVisitor::visitImpl(InputText& inputText) {
    
}





}    // namespace gameSpecification::rule


// test driver code
using namespace gameSpecification::rule;
using gameSpecification::printExpVisitor;

void
testTokenizeExpression(GameState state) {
    InterpretVisitor interpreter(state);
    auto testStr = "players.elements.collect(player, player.weapon.contains(x)).size.contains(weapon)";
    auto names   = interpreter.tokenizeDotNotation(testStr);
    for (auto s : names) {
        std::cout << s << std::endl;
    }
    /*
    output:
        players
        elements
        collect(player, player.weapon.contains(x))
        size
        contains(weapon)
    */
}

void
testEvaluateExpression(GameState state) {
    Expression exp1 = ExpMap({ { { "c", 1234 } } });
    Expression exp2 = ExpMap({ { { "b", exp1 } } });
    Expression exp3 = ExpMap({ { { "a", exp2 } } });
    state.constants.map["root_key"] = exp3;
    
    Expression objectName = std::string("root_key.a.b.c");

    InterpretVisitor interpreter(state);
    auto res = interpreter.evaluateExpression(objectName);
    boost::apply_visitor(printExpVisitor(), res);

    // output: 1234
}



//test interpreter fn's
int main() {
    GameState state;
    std::cout << "------------ testTokenizeExpression --------------" << std::endl;
    testTokenizeExpression(state);
    std::cout << "------------ testEvaluateExpression --------------" << std::endl;
    testEvaluateExpression(state);
    return 0;
}