#pragma once

#include "MapWrapper.h"

#include <boost/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/polymorphic_get.hpp>
#include <iostream>
#include <string_view>

namespace gameSpecification {

// An Expression is a R-value in game specification

struct ExpMap;
struct ExpList;


using Expression = boost::variant<
    boost::recursive_wrapper<ExpMap>,
    boost::recursive_wrapper<ExpList>,
    int,
    bool,
    std::string>;


struct ExpMap {
    MapWrapper<std::string, Expression> map;
};


struct ExpList {
    std::vector<Expression> list;
    
    size_t
    getSize() { return list.size(); }
};


template<class SubTy>
SubTy
castExp(const Expression& exp) {
    SubTy res;
    try {
        res = boost::polymorphic_strict_get<SubTy>(exp);
    } catch (boost::bad_get e) {
        throw std::runtime_error("invalid castExp");
    }
    return res;
}

// returns mutable subTy
template<class SubTy>
SubTy&
castExpUnsafe(Expression& exp) {
    try {
        SubTy& res = boost::polymorphic_strict_get<SubTy>(exp);
        return res;
    } catch (boost::bad_get e) {
        throw std::runtime_error("invalid castExpUnsafe");
    }
}

// For debugging purposes
class printExpVisitor : public boost::static_visitor<void> {
public:
    // void operator()(const std::string& str) const { }

    void operator()(int i) const {
        std::cout << i;
    }

    void operator()(const std::string& str) const {
        std::cout << str;
    }

    void operator()(bool b) const {
        std::cout << b;
    }

    void operator()(const ExpMap& expMap) const {
        std::cout << "{ ";
        for (const auto& [key, exp] : expMap.map) {
            std::cout << key << ": ";
            boost::apply_visitor(*this, exp);
            std::cout << ", ";
        }
        std::cout << "}";
    }

    void operator()(const ExpList& expList) const {
        std::cout << "[ ";
        for (const auto& exp : expList.list) {
            boost::apply_visitor(*this, exp);
            std::cout << ", ";
        }
        std::cout << " ]";
    }
};


}    // namespace gameSpecification
