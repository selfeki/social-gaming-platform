#pragma once

#include "MapWrapper.h"
#include <boost/variant.hpp>

#include <boost/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>

namespace gameSpecification {


// An Expression is a R-value in game specification

struct ExpMap;
struct ExpList;

using Expression = boost::variant<
    int,
    bool,
    std::string,
    boost::recursive_wrapper<ExpMap>,
    boost::recursive_wrapper<ExpList>
    >;

struct ExpMap {
    MapWrapper<std::string, Expression> map;
};

struct ExpList {
    std::vector<Expression> list;
};


}   // namespace gameSpecification
