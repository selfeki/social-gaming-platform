#include "arepa/game_spec/GameSpecification.h"

#include <GameManager.hpp>

using namespace gameSpecification;


int main(int argc, char* argv[]) {

    // std::cout << "Adding values from [1,2,3,4] into 'sum'\n";

    // Expression elemList = ExpList({{1,2,3,4}});
    // Expression elem = std::string_view("number");
    // auto forEachRule = new rule::ForEach(elemList, elem);

    // Expression to = std::string_view("output.sum");
    // Expression value = std::string_view("number");
    // auto addRule = new rule::Add(to, value);

    // forEachRule->next = NULL;
    // forEachRule->next_nested = addRule;
    // addRule->next = NULL;
    // addRule->next_nested = NULL;


    // rule::RuleList ruleList = {forEachRule, addRule};

    // GameState gs;
    // gs.constants = {};
    // gs.variables = {
    //                  {"output", ExpMap({{{"sum", 0},{"truth", 42}}}) }
    //                 };
    // gs.perAudience = {};
    // gs.perPlayer = {};

    // GameInstance gameInstance {gs, ruleList, *forEachRule};

    // gameInstance.updateState();
    // gameInstance.testPrintVariable("output.sum");
    // std::cout << "\n";

    return 0;
}