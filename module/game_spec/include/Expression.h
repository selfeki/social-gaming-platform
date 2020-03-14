#pragma once

#include "MapWrapper.h"
#include <iostream>

#include <boost/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>
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
    std::string_view>;

struct ExpMap {
    MapWrapper<std::string_view, Expression> map;
};

struct ExpList {
    std::vector<Expression> list;
};

// For debugging purposes
class printExpVisitor : public boost::static_visitor<void> {
public:

    // void operator()(const std::string& str) const { }

    void operator()(int i) const {
        std::cout << i;
    }

    void operator()(const std::string_view str) const {
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
