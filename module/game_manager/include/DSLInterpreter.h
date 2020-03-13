#pragma once

#include "arepa/game_spec/GameState.h"
#include "arepa/game_spec/Rule.h"

#include <stack>

namespace gameSpecification::rule {

struct ObjectPointer {
    std::vector<std::string_view> indices;

    // I'm not sure if this is gonna work, needs testing
    // I'm trying to dereference the pointer
    // https://stackoverflow.com/questions/550859/reference-to-value-of-stl-map-element

    std::optional<ExpMap>
    getExpMap(const Expression& exp) {
        const ExpMap* res;
        try {
            res = &boost::get<ExpMap>(exp);
        } catch(boost::bad_get e) {
            return { };
         }
        return *res;
    }

    std::optional<Expression>
    dereferenceFromContext(const ExpMap& expMap) {
        Expression exp = expMap;
        for (const auto& ind : indices) {
            auto nestedMap = getExpMap(exp);
            if (!nestedMap) {
                return { };
            }
            auto m = nestedMap.value().map;
            auto it = m.find(ind);
            if (it == m.end()) { 
                return { };
            }
            exp = it->second;
        }
        return exp;
    }
};

class InterpretVisitor : public RuleVisitor {
public:
    InterpretVisitor(GameState& state)
        : state { state }
        , needUserInput { false } {}

    void visitImpl(const rule::ForEach&);

    void visitImpl(const rule::GlobalMessage&);

    void visitImpl(const rule::InputChoice&);

    void
    setGameState(const GameState&);

private:
    GameState& state;

    bool needUserInput;

    std::stack<const Rule*> ruleStack;
};

}    // namespace gameSpecification::rule