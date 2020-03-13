// A stub namespace to silence a couple CMake warnings.

#include "Expression.h"
#include "ExpressionPtr.h"
#include <boost/variant/polymorphic_get.hpp>


#include <iostream>
#include <string>

using namespace gameSpecification;

// todo: transfer these to test module once Google Test gets integrated

//  Driver for testing ExpressionPtr
void 
testExpressionPtr() {
    Expression exp1 = ExpMap({ { { "c", 1 } } });
    Expression exp2 = ExpMap({ { { "b", exp1 } } });
    Expression exp3 = ExpMap({ { { "a", exp2 } } });
    ExpMap& mapRef  = boost::polymorphic_strict_get<ExpMap>(exp3);
    ExpMap mapCopy  = boost::polymorphic_strict_get<ExpMap>(exp3);

    ExpressionPtr expPtr{{"a", "b", "c"}};
    auto PtrCopy = expPtr.getPtr(mapCopy);
    auto PtrRef  = expPtr.getPtr(mapRef);

    if (!PtrCopy) {
        std::cout << "null";
    } else {
        *(PtrCopy.value()) = 42;
        // exp3 = { a: { b: { c: 1, }, }, }
        boost::apply_visitor(printExpVisitor(), exp3);
    }
    std::cout << std::endl;

    if (!PtrRef) {
        std::cout << "null";
    } else {
        *(PtrRef.value()) = 42;
        // exp3 = { a: { b: { c: 42, }, }, }
        boost::apply_visitor(printExpVisitor(), exp3);
    }
}

//  Example usage of boost::variant and boost::apply_visitor
void 
testPrintExpVisitor() {
    Expression intExp = 0;
    boost::apply_visitor(printExpVisitor(), intExp);
    std::cout << std::endl;

    Expression boolExp = true;
    boost::apply_visitor(printExpVisitor(), boolExp);
    std::cout << std::endl;

    // Expression exp3 = "hello";
    // does not work; "hello" gets implicitly converted to bool
    // see https://stackoverflow.com/questions/13268608/boostvariant-why-is-const-char-converted-to-bool
    Expression stringExp = std::string_view("hello");
    boost::apply_visitor(printExpVisitor(), stringExp);
    std::cout << std::endl;

    Expression expMap = ExpMap({ { { "a", 1 } } });
    boost::apply_visitor(printExpVisitor(), expMap);
    std::cout << std::endl;

    Expression expList = ExpList({ { 1, false, std::string_view("hello") } });
    boost::apply_visitor(printExpVisitor(), expList);
    std::cout << std::endl;

    Expression nestedMap = ExpMap({ { { "a", expMap } } });
    boost::apply_visitor(printExpVisitor(), nestedMap);
    std::cout << std::endl;

    Expression nestedList = ExpList({ { expList, std::string("hello"), nestedMap, 1, false } });
    boost::apply_visitor(printExpVisitor(), nestedList);
    std::cout << std::endl;
}


int main() {
    std::cout << "------------ testPrintExpVisitor --------------" << std::endl;
    testPrintExpVisitor();
    std::cout << "------------ testExpressionPtr --------------" << std::endl;
    testExpressionPtr();
    return 0;
}