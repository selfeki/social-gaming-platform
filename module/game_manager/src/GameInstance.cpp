#include "GameInstance.h"

#include <string>
#include <vector>


using Rules = rules::RuleList

using namespace GameInstance;

GameInstance(Rules& rules)
  : interpreter{rules}
    { }

// Critical design issue: how to handle interpreting rules that require user input?
// Issue is resuming the traversal of the AST from where we paused.
void 
updateState() {
  // interpreter.interpret();
}