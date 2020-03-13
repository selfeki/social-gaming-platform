#pragma once

#include "arepa/game_spec/GameState.h"
#include "arepa/game_spec/Rule.h"

#include <boost/variant/polymorphic_get.hpp>
#include <stack>


using namespace gameSpecification;


// todo: move this somewhere else
struct ObjectPointer {
    std::vector<std::string_view> indices;

    // I'm not sure if this is gonna work, needs testing
    // I'm trying to dereference the pointer
    // https://stackoverflow.com/questions/550859/reference-to-value-of-stl-map-element

    std::optional<ExpMap*>
    getExpMap(Expression& exp) {
        ExpMap* res;
        try {
            res = &boost::polymorphic_strict_get<ExpMap>(exp);
        } catch(boost::bad_get e) {
            return { };
         }
        return res;
    }

    std::optional<Expression*>
    dereferenceFromContext(ExpMap& expMap) {
        std::cout << &(expMap.map["a"]) << std::endl;
        ExpMap* exp = &expMap;
        std::cout << &(exp->map["a"]) << std::endl;
        for (const auto& ind : indices) {
            // const auto& m = mapPtr.value()->map;
            // check if ind is a key in the expMap
            auto it = exp->map.find(ind);
            if (it == exp->map.end()) { 
                return { };
            }
            // std::cout << "found " << ind << std::endl;

            Expression& temp= it->second;
            std::cout << ind << " " << indices[indices.size()-1] << std::endl;

            assert(indices.size() > 0);
            if (&ind == &indices.back()) { return &temp; }

            // std::cout << ind << " " << indices[indices.size()-1] << std::endl;

            // check if found Expression is an expMap 
            auto mapPtr = getExpMap(temp);
            if (!mapPtr) {
                return { };
            }
            exp = mapPtr.value();
        }
        return { };
    }
};


namespace gameSpecification::rule {


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