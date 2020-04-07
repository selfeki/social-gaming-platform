#include "DSLInterpreter.h"
#include "arepa/game_spec/ExpressionPtr.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string_view>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/any.hpp>

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

std::string
insideParenthesis(const std::string_view str){
    std::size_t openParentheses = str.find("(");
    int digitStart = openParentheses+1;
    std::size_t closeParentheses = str.find(")");
    auto extracted = str.substr(digitStart,closeParentheses-digitStart);
    return std::string(extracted);
}

bool
isAllDigits(const std::string_view str){
    if(str.end() == std::find_if(str.begin(), str.end(),
            [](unsigned char c)->bool {return !isdigit(c);})){
        return true;
    }
    return false;
}

Expression
applySizeAttribute(const Expression& exp){
    try{
        auto temp = castExp<ExpList>(exp);
        int size = temp.getSize();
        Expression ret = Expression(size);
        return ret;
    }
    catch(const std::exception& e){
        std::cout<<"something wrong in size";
    }
}

Expression
applyUpFromAttribute(const Expression& exp, std::string attr){
    std::vector<Expression> resVec = std::vector<Expression>();
    if(exp.which() !=2 ) {
        std::cout << "something wrong in upfrom";
        //throw some error
    }
    else{
        auto substr = insideParenthesis(attr);
        int lowerLim = boost::lexical_cast<int>(substr);
        int upperLim = castExp<int>(exp);
        for(int i = lowerLim; i <= upperLim; i++){
            Expression num = Expression(i);
            resVec.push_back(num);
        }
    }
    ExpList retList = ExpList({resVec});
    return Expression(retList);
}

Expression
applyContainsAttribute(const Expression& exp, std::string attr){
    //check to see if exp is ExpList
    if(exp.which() !=1 ) {
        std::cout << "something wrong in contains";
        //throw some error
    }
    else {
        auto attrExpList = castExp<ExpList>(exp).list;
        auto containsParam = insideParenthesis(attr);
        //comparing ints
        if(isAllDigits(containsParam)){
            int containsParamAsInt = boost::lexical_cast<int>(containsParam);
            //boost::apply_visitor(printExpVisitor(),exp);
            for(const auto & elem : attrExpList){
                //hardcoded for int POC
                if (boost::get<int>(elem) == containsParamAsInt){
                    return Expression(true);
                }
            }
        }
        //comparing strings
        //WIP
        else{
            for(const auto & elem : attrExpList){
                auto elemStr = castExp<ExpString>(elem);
                if(elemStr.getString() == containsParam){
                    return Expression(true);
                }
            }
        }
    }
    return Expression(false);
}

Expression
applyCollectAttribute(const Expression& exp, std::string_view attr){
/*
    auto result = ExpList();
    //auto localScope = exp.top();
    ExpList tempres = ExpList();
    //for(){
    auto innerExp = insideParenthesis(attr);
    auto exphp = Expression(innerExp);
    auto evalRes = evaluateExpression(exphp);
    auto boolRes = castExp<bool>(evalRes);
    if(boolRes){
        //result.list.push_back();
    }
    //}*/
    return Expression("fakeExpression");

}


Expression
applyAttribute(const Expression& exp, std::string attr) {
    // todo
    if (attr.find("size")!=std::string::npos){
       return applySizeAttribute(exp);
    }
    else if(attr.find("upfrom")!=std::string::npos){
        return applyUpFromAttribute(exp, attr);
    }
    else if(attr.find("contains")!=std::string::npos){
        return applyContainsAttribute(exp, attr);
    }
    else if (attr.find("collect")!=std::string::npos){
        return applyCollectAttribute(exp, attr);
    }
    std::cout<<"attr not recognized";
    return 0;
}


namespace gameSpecification::rule {


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
    //assert(exp.type() != typeid(std::string));
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

void testSizeAttr(){
    auto explist = ExpList();
    Expression expList1 = Expression(explist);
    std::cout << "empty test\n";
    auto test1 = applyAttribute(expList1, ".size");
    if(boost::get<int>(test1) == 0){
        std::cout << "size Sucessful. found: " << boost::get<int>(test1);
    }
    else{
        std::cout << "size Unsucessful. found: " << boost::get<int>(test1) << ". Expected 0\n";
    }

    Expression s1 = Expression(1);
    auto list = std::vector<Expression>();
    list.push_back(s1);
    explist.list = list;
    Expression expList2 = Expression(explist);
    std::cout<<"\nsingle Expression Test\n";
    auto test2 = applyAttribute(expList2, ".size");
    if(boost::get<int>(test2) == 1){
        std::cout << "size Successful. found: " << boost::get<int>(test2);
    }
    else{
        std::cout << "size Unsucessful. found: " << boost::get<int>(test2) << ". Expected 1\n";
    }
    Expression s2 = Expression(2);
    Expression s3 = Expression(3);
    Expression s4 = Expression(4);
    list.push_back(s2);
    list.push_back(s3);
    list.push_back(s4);
    explist.list = list;
    Expression expList3 = Expression(explist);
    std::cout<<"\nMulti Expression Test\n";
    auto test3 = applyAttribute(expList3, ".size");
    if(boost::get<int>(test3) == 4){
        std::cout << "size Successful. found: " << boost::get<int>(test3);
    }
    else{
        std::cout << "size Unsuccessful. found: " << boost::get<int>(test3) << ". Expected 4\n";
    }
}


void testContainsAttr(){
    std::cout << "***ints list test***\n";
    Expression s1 = Expression(1);
    Expression s2 = Expression(2);
    Expression s3 = Expression(3);
    auto list = std::vector<Expression>();
    list.push_back(s1);
    list.push_back(s2);
    list.push_back(s3);
    auto explist = ExpList();
    explist.list = list;
    Expression listAsExp = Expression(explist);
    std::string type = ".contains(3)";
    std::cout << "contains positive test\n";
    auto test = applyAttribute(listAsExp, type);
    if (boost::get<bool>(test)){
        std::cout << "Contains successful\n";
    }
    else{
        std::cout << "Contains unsuccessful: 3 no found\n";
    }
    type = ".contains(6)";
    auto test2 = applyAttribute(listAsExp, type);
    std::cout << "contains negative test\n";
    if (!boost::get<bool>(test2)){
        std::cout << "Contains successful \n";
    }
    else{
        boost::apply_visitor(printExpVisitor(), test);
        std::cout << "Contains unsuccessful: some how found a value\n";
    }

    std::cout << "***strings list test***\n";
    ExpString str1 = ExpString();
    str1.str = "apple";
    ExpString str2 = ExpString();
    str2.str = "banana";
    ExpString str3 = ExpString();
    str3.str = "coconut";

    Expression expS1 = Expression (str1);
    Expression expS2 = Expression (str2);
    Expression expS3 = Expression (str3);

    auto strlist = std::vector<Expression>();
    strlist.push_back(expS1);
    strlist.push_back(expS2);
    strlist.push_back(expS3);

    auto strListAsExpList = ExpList();
    strListAsExpList.list = strlist;

    std::cout << "contains positive test\n";
    Expression toApplyContains = Expression(strListAsExpList);
    auto positiveTest = applyAttribute(toApplyContains, ".contains(coconut)");
    if (boost::get<bool>(positiveTest)){
        std::cout << "Contains successful\n";
    }
    else{
        std::cout << "Contains unsuccessful. coconut not found\n";
    }
    std::cout << "contains negative test\n";
    auto negativeTest = applyAttribute(toApplyContains, ".contains(durian)");
    if (!boost::get<bool>(negativeTest)){
        std::cout << "Contains successful\n";
    }
    else{
        std::cout << "Contains unsuccessful. durian was found... but shouldn't have been\n";
    }
}

void testUpFromAttr(){
    std::cout <<"contains test 1: populated array\n";
    Expression s1 = Expression(7);
    std::string op1 = ".upfrom(2)";
    auto test1 = applyAttribute(s1, op1);
    std::cout << "expected: [2,3,4,5,6,7]\nactual: ";
    boost::apply_visitor(printExpVisitor(),test1);

    std::cout <<"\ncontains test 2: single element array\n";
    std::string op2 = ".upfrom(7)";
    auto test2 = applyAttribute(s1, op2);
    std::cout << "expected: [7]\nactual: ";
    boost::apply_visitor(printExpVisitor(),test2);

    std::cout <<"\ncontains test 3: upfrom val too high\n";
    std::string op3 = ".upfrom(8)";
    auto test3 = applyAttribute(s1, op3);
    std::cout << "expected: []\nactual: ";
    boost::apply_visitor(printExpVisitor(),test3);
}

//test interpreter fn's
int main() {
    GameState state;
    std::cout << "------------ testTokenizeExpression --------------" << std::endl;
    testContainsAttr();

    std::cout << "\n------------ testEvaluateExpression --------------" << std::endl;
    return 0;

}