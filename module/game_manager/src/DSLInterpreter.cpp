#include "DSLInterpreter.h"
#include "arepa/game_spec/ExpressionPtr.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string_view>


namespace gameSpecification::rule {


// converts string "players.weapon" to a vector {"players", "weapon"}
std::vector<std::string_view>
InterpretVisitor::tokenizeDotNotation(std::string_view str) {
    std::vector<std::string_view> names;
    std::stringstream ss(static_cast<std::string>(str));
    std::string buffer;
    while (std::getline(ss, buffer, '.')) {
        names.push_back(buffer);
    }
    return names;
}

std::optional<Expression>
InterpretVisitor::lookupLocalScope(const std::vector<std::string_view>& indices) {
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
InterpretVisitor::lookupGlobalScope(const std::vector<std::string_view>& indices) {
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
InterpretVisitor::lookup(std::string_view name) {
    auto indices       = tokenizeDotNotation(name);
    auto expLocalScope = lookupLocalScope(indices);
    if (expLocalScope) {
        return expLocalScope;
    }
    return lookupGlobalScope(indices);
}

std::string_view
InterpretVisitor::interpolateString(const std::string_view str) {
    // parses strings like
    // "Round {round}. Choose your weapon!"
    // to "Round 1. Choose your weapon!"
    return std::string_view("");
}

Expression
InterpretVisitor::evaluateExpression(const Expression& exp) {
    // parses string like
    // "configuration.Rounds.upfrom(1)"
    // into an actual expression
    
    /* Some useful grammar productions:
        name -> a  string
        listAttribute -> size | elements.name | elements.contains | elements.collect
            (note: elements.name only applies to list of maps)
        intAttribute  -> upfrom(1)
        attribute -> listAttribute | intAttribute
        names -> name(.name)*(.attribute)*
    */

    if (exp.type() != typeid(std::string_view)) {
        // the value is a literal, nothing to evaluate
        return exp;
    }
    auto str = castExp<std::string_view>(exp);

    // todo: handle attributes

    // todo: finish easy case
}

void 
InterpretVisitor::visitImpl(InputChoice& rule) {
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

void 
InterpretVisitor::visitImpl(ForEach& forEach) {
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

        state.context.back().map.erase(element);

        return;
    } else {
        forEach.nestedRulesInProgess = true;
    }

    auto exp = elemList.list[forEach.elemListIndex];
    state.context.back().map[element] = exp;

    forEach.elemListIndex += 1;
}

void 
InterpretVisitor::visitImpl(Add& add) {
    auto toExp       = castExp<std::string_view>(add.to);
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
    add.finished = true;
}

void 
InterpretVisitor::visitImpl(GlobalMessage& globalMessage) {
    auto content = castExp<std::string_view>(globalMessage.content);
    auto message = interpolateString(content);
    // todo;
}

void 
InterpretVisitor::visitImpl(InputText& inputText) {
    // todo
}


}    // namespace gameSpecification::rule
