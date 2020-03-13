
#include "ExpressionPtr.h"

#include <optional>
#include <boost/variant/polymorphic_get.hpp>

ExpressionPtr::ExpressionPtr(std::vector<std::string_view> inds)
    : indices{inds}
    { }

std::optional<ExpMap*>
castToExpMap(Expression& exp) {
    ExpMap* res;
    try {
        res = &boost::polymorphic_strict_get<ExpMap>(exp);
    } catch(boost::bad_get e) {
        return { };
        }
    return res;
}


std::optional<Expression*>
ExpressionPtr::getPtr(ExpMap& expMap) {
    assert(indices.size() > 0);
    ExpMap* exp = &expMap;
    for (const auto& ind : indices) {
        // check if ind is a key in the expMap
        auto it = exp->map.find(ind);
        if (it == exp->map.end()) { 
            return { };
        }
        Expression& temp= it->second;
        if (&ind == &indices.back()) { return &temp; }
        // check if found Expression is an expMap 
        auto mapPtr = castToExpMap(temp);
        if (!mapPtr) {
            return { };
        }
        exp = mapPtr.value();
    }
    return { };
}