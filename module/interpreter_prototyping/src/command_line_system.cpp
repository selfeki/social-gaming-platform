#include "GameSpecification.h"

using namespace gameSpecification;


int main(int argc, char* argv[]) {

    Expression elemList = ExpList({{1,2,3,4}});
    Expression elem = std::string_view("number");

    auto forEachRule = rule::ForEach {elemList, elem};

    Expression to = std::string_view("sum");
    Expression value = std::string_view("number");

    auto addRule = rule::Add {to, value};

    forEachRule.next = NULL;
    forEachRule.next_nested = &addRule;

    addRule.next = NULL;

    


    return 0;
}