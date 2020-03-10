// A stub namespace to silence a couple CMake warnings.

#include "Expression.h"
#include <iostream>
#include <string>

using namespace gameSpecification;

namespace game_spec_stub {    // NOLINT
void foo() {}                 // NOLINT
}

//  Example usage of boost::variant and boost::apply_visitor

int main() {
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

    Expression expMap = ExpMap({ {{"a", 1}} });
    boost::apply_visitor(printExpVisitor(), expMap);
    std::cout << std::endl;

    Expression expList = ExpList({ {1, false, std::string_view("hello")} });
    boost::apply_visitor(printExpVisitor(), expList);
    std::cout << std::endl;

    Expression nestedMap = ExpMap({ {{"a", expMap}} });
    boost::apply_visitor(printExpVisitor(), nestedMap);
    std::cout << std::endl;

    Expression nestedList = ExpList({ {expList, std::string("hello"), nestedMap, 1, false} });
    boost::apply_visitor(printExpVisitor(), nestedList);
    std::cout << std::endl;

    return 0;
}
