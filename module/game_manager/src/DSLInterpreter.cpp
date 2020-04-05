#include "DSLInterpreter.h"
#include "arepa/game_spec/ExpressionPtr.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string_view>
#include <sstream>
#include <random>
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
tokenizeDotNotation(std::string_view str) {
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

using typeVar = boost::variant<std::string, int, bool>;

// evaluates if '==', '<' and '>' Caution!! only pass bool, int, string Expressions 
bool 
evaluateEquality(const Expression exp1, const Expression exp2, const std::string comp){
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
         v1 = castExp<std::string>(exp1);
        v2 = castExp<std::string>(exp2);
    }
    
    if(comp == "==")
        return v1==v2;
    else if(comp == "<")
        return v1 < v2;
    else if(comp == "!=")
        return v1 != v2 ;
    else
        return v1 > v2;
}

std::vector<std::string> tokenizer(std::string str, std::string symbol){
	int start;
	int end = 0;
    std::vector<std::string> output ;
	while ((start = str.find_first_not_of(symbol, end)) != std::string::npos){
        end = str.find(symbol, start);
		output.push_back(static_cast<std::string>(str.substr(start, end - start)));
	}
    return output ;
}
// returns the list attribute after evaluating the 'element', 'Collect' condition
// {list name, attribute of list, 'contains'/'collect', contains attribute/ collect conditions}
// e.g players.elements.collect(player, player.weapon == weapon.beats)

// return a Bool after evaluaing the 'contains' condition
bool
evalListContains(const ExpList list, Expression attribute, Expression toCompareTo){
    auto it = list.list.begin();
    while(it != list.list.end()){
        ExpMap map = castExp<ExpMap>(*it);
        auto playerVariableMap = (map.map.begin())->second;
        auto itr  = castExp<ExpMap>(playerVariableMap).map.find(
                            castExp<std::string>(attribute));
        if(itr != castExp<ExpMap>(playerVariableMap).map.end()){
            if(evaluateEquality(itr->second, toCompareTo, "==")){
                return true ;
            }
        }
        it++;
    }
    return false;
}

// returns a ExpList of list.element.attribute 
ExpList
getlistAttribute(const ExpList list, Expression attribute){
    ExpList result;
    auto it = list.list.begin();
    while(it != list.list.end()){
        ExpMap mapper = castExp<ExpMap>(*it);
        ExpMap actualMap; 
        auto itr = mapper.map.find(castExp<std::string>(attribute));
        if(itr != mapper.map.end()){
            actualMap = mapper;
        }else{
            auto itr  = mapper.map.begin();
            actualMap = castExp<ExpMap>(itr->second); // map1
        }
        auto itrToAttribute = actualMap.map.find(castExp<std::string>(attribute));
        if(itrToAttribute != actualMap.map.end()){
            result.list.push_back(itrToAttribute->second);
        }
        it++;
    }
    return result ;
}

//returns a ExpLsit of list.elements.collect
ExpList
getCollectList(ExpList listLhs, std::string condition, ExpMap rhsMap){
    // Break up the condition
    std::string stringCondition = castExp<std::string>(condition);
    auto commaIndex = stringCondition.find(",");
    auto toCollect = stringCondition.substr(0, commaIndex-1);
    std::string comparison;
    ExpList result ;
    for(auto comp: {"<", "==", "<", "!="}){
        if(stringCondition.find(comp)){
            comparison = stringCondition.substr(stringCondition.find_first_of(comp),
                                            stringCondition.find_last_of(comp));
            break;;
        }
    }
    auto lhs = stringCondition.substr(commaIndex+1, stringCondition.find(comparison)-2);
    auto rhs = stringCondition.substr(stringCondition.find_last_of(comparison)+1, stringCondition.size()-1);
    auto tokenizedLhs = tokenizer(lhs, ".");
    auto tokenizedRhs = tokenizer(rhs, ".");
    auto itrList = listLhs.list.begin();
    while(itrList!=listLhs.list.end()){
        auto map = castExp<ExpMap>(*itrList);
        auto lhsvariableItr = map.map.find(tokenizedLhs[1]);
        auto rhsVariableItr = rhsMap.map.find(tokenizedRhs[0]);
        if(lhsvariableItr == map.map.end() || rhsVariableItr == rhsMap.map.end()){
            // Todo :: report error
            return result ;
        }
        if(evaluateEquality(lhsvariableItr->second, rhsVariableItr->second, comparison)){
            result.list.push_back(map);
        }
        itrList++;
    }
    return result;
}

namespace gameSpecification::rule {

std::vector<std::string>
evaluatelistExp(const Expression list){
    auto listString = castExp<std::string>(list);
    auto openingBrackIndex = listString.find("(");
    auto beforeBracket = tokenizer(listString.substr(0, openingBrackIndex), ".");
    auto actualList = static_cast<std::string>(beforeBracket[0]);
    auto attribute = beforeBracket[2];
    std::string contains_collect;
    if(attribute == "collect"){
        contains_collect = beforeBracket[2];
    }else if(beforeBracket.size()>3){
        contains_collect = beforeBracket[3];
    }else{
        contains_collect = "none";
    }
    std::string condition = "none";
    if(openingBrackIndex !=std::string::npos ){
        auto end = listString.length() ;
        condition = listString.substr(openingBrackIndex+1, listString.length()-2);
        // sub string does not eliminae the last string so using pop back
        condition.pop_back();
    }
    std::vector<std::string> result{actualList, attribute, contains_collect, condition};
    return result;

}
std::optional<Expression*>
InterpretVisitor::lookupLocalScope(const std::vector<std::string>& indices) {
    ExpressionPtr expPtr(indices);
    auto& context = state.context;
    auto found = std::find_if(context.rbegin(), context.rend(),
        [&expPtr](auto& expMap) { return expPtr.getPtr(expMap); }
    );
    if (found == context.rend()) {
        return { };
    }
    auto res = expPtr.getPtr(*found);
    return res;
}

std::optional<Expression*>
InterpretVisitor::lookupGlobalScope(const std::vector<std::string>& indices) {
    ExpressionPtr expPtr(indices);
    auto ptrVars = expPtr.getPtr(state.variables);
    if (ptrVars) {
        return ptrVars;
    }
    auto ptrConsts = expPtr.getPtr(state.constants);
    return ptrConsts;
}

std::optional<Expression*>
InterpretVisitor::lookupWithIndices(const std::vector<std::string>& indices) {
    auto expLocalScope = lookupLocalScope(indices);
    if (expLocalScope) {
        return expLocalScope;
    }
    return lookupGlobalScope(indices);
}

std::optional<Expression*>
InterpretVisitor::lookupName(std::string_view name) {
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
    auto resExp = *(lookupRes.value());
    // apply attributes
    for (const auto& attr : attributes) {
        resExp = applyAttribute(resExp, attr);
    }
    return resExp;
}

// todo: refactor redundancy with evaluateExpression
// used to get pointers to objects in the state (needed for input rules).
// This functions makes explicit that fact that the returned expression is
// a pointer to an expression in state
std::optional<Expression*>
InterpretVisitor::getExpressionPtr(const Expression& name) {
    assert(exp.type() != typeid(std::string));
    auto str = castExp<std::string>(name);
    return lookupName(str);
}

InputRequest
InterpretVisitor::createInputRequest(InputType type,
                   const Expression& targetUser,
                   const Expression& promptExp,
                   const Expression& choiceList,
                   const Expression& resultExp,
                   const std::optional<Expression>& timeout) {
    // check if the targetUser field is valid
    auto exp = evaluateExpression(targetUser);
    auto user = castExp<ExpMap>(exp);
    assert(user.map.count("name") > 0);

    auto rawPrompt = castExp<std::string>(promptExp);
    auto prompt = interpolateString(rawPrompt);
    // extract list of strings as choices
    auto choicesExp = evaluateExpression(choiceList);
    auto choiceExpList = castExp<ExpList>(choicesExp).list;
    std::vector<std::string> choices;
    std::transform(choiceExpList.begin(), choiceExpList.end(), choices.begin(),
        [](const auto& exp) { return castExp<std::string>(exp); }
    );
    // extract pointer to result
    auto expPtr = getExpressionPtr(resultExp);
    if (!expPtr) {
        throw std::runtime_error("invalid result object in ");
    }
    auto* resultPtr = expPtr.value();
    // todo: handle InputVote case
    // result must by a map containing all entries in choices as keys

    // extract timeout if provided
    std::optional<int> tOut = {};
    if (timeout) {
        auto timeoutExp = evaluateExpression(timeout.value());
        tOut = castExp<int>(timeoutExp);
    }

    return InputRequest({type, user, prompt, choices, resultPtr, tOut});
}

// load gamestate with input request details
void 
InterpretVisitor::visitImpl(InputChoice& rule) {
    auto inputRequest = createInputRequest(InputType::CHOICE, 
                                           rule.targetUser, 
                                           rule.prompt, 
                                           rule.choiceList, 
                                           rule.result, 
                                           rule.timeout);
    // todo:
    // set flag indicating need for user input
    // needUserInput = true;
    state.enqueueInputRequest(inputRequest);
}

// todo: refactor redundancy
void 
InterpretVisitor::visitImpl(InputText& rule) {
    auto inputRequest = createInputRequest(InputType::TEXT, 
                                           rule.targetUser, 
                                           rule.prompt, 
                                           {}, 
                                           rule.result, 
                                           rule.timeout);
    state.enqueueInputRequest(inputRequest);
}

// void 
// InterpretVisitor::visitImpl(InputVote& rule) {
//     // extract pointer to result
//     auto resultStr = castExp<std::string>(rule.resultMap);
//     auto indices = tokenizeDotNotation(resultStr);
//     ExpressionPtr resultPtr(indices);

//     for (const auto& user : rule.targetUsers) {
//         auto inputRequest = createInputRequest(InputType::VOTE, 
//                                                rule.targetUser, 
//                                                rule.prompt, 
//                                                rule.choiceList, 
//                                                resultMap, 
//                                                rule.timeout);
//         state.enqueueInputRequest(inputRequest);
//     }
// }

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

// todo: make this use getExpressionPtr
// the add rule needs to contain a pointer to an expression instead
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
    auto message = interpolateString(globalMessage.content);
    state.enqueueGlobalMessage(std::move(message));
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
void InterpretVisitor::visitImpl(Reverse& reverse){
    // name of the list
    auto listName = castExp<std::string>(reverse.list);  
    // search for list in variabels
    auto currentState = this->getGameState();
    auto it = currentState.variables.map.find(listName);
    if(it == currentState.variables.map.end()){
        // todo:: Report Error - list not Found
        return;
    }
    auto secondList = castExp<ExpList>(it->second) ;
    std::reverse(secondList.list.begin(), 
                                    secondList.list.end());
    auto li  = secondList.list.begin();
    currentState.variables.map.at(listName) = secondList;
    // update state
    this->setGameState(currentState);
}
void InterpretVisitor::visitImpl(Shuffle& shuffle){
    // name of list 
    auto listName = castExp<std::string>(shuffle.list);
    // search for list in state 
    auto currentState = this->getGameState();
    auto it = currentState.variables.map.find(listName);
    if(it == currentState.variables.map.end()){
        // todo :: return some error
        return;
    }
    // random number generator
    auto myrandom = std::default_random_engine {};
    auto secondList = castExp<ExpList>(it->second);
    std::shuffle(secondList.list.begin(), 
                                secondList.list.end(), myrandom);
    // update state
    currentState.variables.map.at(listName) = secondList;
    this->setGameState(currentState);
}

// evaluate Elements and Collect
ExpList getEvaluatedList(std::vector<std::string>listExpression, GameState state){
    auto listName = listExpression[0];
    ExpList actualList;
    std::cout<<"List name is -> "<<listName<<std::endl;
    auto it = state.variables.map.find(listName);
    // // check if 'Collect' list expression
    if(it == state.variables.map.end()){
        //todo :: report error
        printf("Not found\n");
        return actualList;
    }
    actualList = castExp<ExpList>(it->second);
    if(listExpression[1]=="collect"){
        actualList = getCollectList(actualList, listExpression[3], state.variables);
    }else{
        actualList = getlistAttribute(actualList, listExpression[1]);
    }
    return actualList;
}

void InterpretVisitor::visitImpl(Discard& discard){
        //name of list 
        auto listExp = castExp<std::string>(discard.fromList);
        std::cout<<"EXP IS "<<listExp<<std::endl;
        auto count  = castExp<int>(discard.count);
        auto currentState = this->getGameState();
        ExpList actualList ;
        if(listExp.find(".") != std::string::npos ){
            auto listExpressionEval = evaluatelistExp(discard.fromList);
            auto actualList = getEvaluatedList(listExpressionEval, currentState);
            for(auto mm: actualList.list){
                std::cout<<castExp<std::string>(mm)<<std::endl;
            }
        }
        else{
            auto itr = currentState.variables.map.find(listExp);
            if(itr != currentState.variables.map.end()){
                actualList = castExp<ExpList>(itr->second);
            }
            else{
                return;
            }
        }
        if(count > actualList.list.size()){
            return;
        }
        while(count > 0 && actualList.list.size() >=1 ){
            std::cout<<"Satnam"<<std::endl;

            actualList.list.erase(actualList.list.begin());
            count--;
        }
        currentState.variables.map.at(listExp) = actualList;
        this->setGameState(currentState);
    }

void InterpretVisitor::visitImpl(rule::Deal& deal){
    auto fromListExpEval = evaluatelistExp(deal.fromList);
    auto toListExpEval = evaluatelistExp(deal.toList);
    auto fromListName = fromListExpEval[0];
    auto toListName = toListExpEval[0];
    auto currentState = this->getGameState();
    auto itF = currentState.variables.map.find(fromListName);
    auto itT = currentState.variables.map.find(toListName);
    auto count  = castExp<int>(deal.count);
    int ctrl = 0;
    ExpList actualToList = castExp<ExpList>(itT->second);
    ExpList actualFromList = castExp<ExpList>(itF->second);
    for(auto list: {actualFromList, actualToList}){
        std::vector<std::string> exprs ;
        if(ctrl){
            exprs = toListExpEval;
        }else{
            exprs = fromListExpEval;
        }
        if(exprs[1]=="collect"){
            list = getCollectList(list, exprs[3], currentState.variables);
        }else{
            list = getlistAttribute(list, exprs[1]);
        }
        // to do :: effeciently parse two list expressions
        ctrl ++;
    }
    if(count > actualFromList.list.size()){
        return;
    }
    while(count > 0){
        auto itr = actualFromList.list.begin() ;
        actualFromList.list.erase(itr);
        actualToList.list.push_back(*itr);
        count--;
    }
    // it->second  = actualList;
    // this->setGameState(currentState);
}


}    // namespace gameSpecification::rule


// test driver code
using namespace gameSpecification::rule;
using gameSpecification::printExpVisitor;

void
testTokenizeExpression(GameState state) {
    InterpretVisitor interpreter(state);
    auto testStr = "players.elements.collect(player, player.weapon.contains(x)).size.contains(weapon)";
    auto names   = tokenizeDotNotation(testStr);
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
testGetExpressionPtr(GameState state) {
    Expression exp1 = ExpMap({ { { "c", 1234 } } });
    Expression exp2 = ExpMap({ { { "b", exp1 } } });
    Expression exp3 = ExpMap({ { { "a", exp2 } } });
    state.constants.map["root_key"] = exp3;
    
    Expression objectName = std::string("root_key.a.b.c");

    InterpretVisitor interpreter(state);
    auto res = interpreter.evaluateExpression(objectName);
    // output: 1234
    boost::apply_visitor(printExpVisitor(), res);
    std::cout << std::endl;

    auto* ptr = interpreter.getExpressionPtr(objectName).value();
    *ptr = 5678;
    res = interpreter.evaluateExpression(objectName);
    // output: 5678
    boost::apply_visitor(printExpVisitor(), res);
    std::cout << std::endl;

}

//test interpreter fn's
// int main() {
//     GameState state;
//     std::cout << "------------ testTokenizeExpression --------------" << std::endl;
//     testTokenizeExpression(state);
//     std::cout << "------------ testEvaluateExpression --------------" << std::endl;
//     testGetExpressionPtr(state);
//     return 0;
// }
