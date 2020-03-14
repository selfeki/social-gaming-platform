
#include "ExpressionPtr.h"

#include <boost/variant/polymorphic_get.hpp>
#include <optional>

ExpressionPtr::ExpressionPtr(std::vector<std::string_view> inds)
    : indices { inds } {}

std::optional<ExpMap*>
castToExpMap(Expression& exp) {
    ExpMap* res;
    try {
        res = &boost::polymorphic_strict_get<ExpMap>(exp);
    } catch (boost::bad_get e) {
        return {};
    }
    return res;
}

/*  
    Basic idea of algorithm:
    
    exp = expMap
    for ind in indices
        exp = exp[ind]
    return &exp
*/
std::optional<Expression*>
ExpressionPtr::getPtr(ExpMap& expMap) {
    assert(indices.size() > 0);
    ExpMap* exp = &expMap;
    for (const auto& ind : indices) {
        // check if ind is a key in the expMap
        auto it = exp->map.find(ind);
        if (it == exp->map.end()) {
            return {};
        }
        // get expression at map[ind]
        // and return it if no more indices
        Expression& temp = it->second;
        if (&ind == &indices.back()) {
            return &temp;
        }
        // check if found Expression is an expMap
        auto mapPtr = castToExpMap(temp);
        if (!mapPtr) {
            return {};
        }
        exp = mapPtr.value();
    }
    return {};
}