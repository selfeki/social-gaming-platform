#include "GameInstance.h"

#include <string>
#include <vector>


// GameInstance::GameInstance(Rules& rules)
//   : interpreter{rules}
//     { }

// Critical design issue: how to handle interpreting rules that require user input?
// Issue is resuming the traversal of the AST from where we paused.

// idea: implement an iterator of rules and iterate rules one by one
// passing each rule to the interpreter and getting back an updated state.
// If the state says user input required then do that before resuming rule interpretation

// The iterator will be implemented using a visitor
void updateState() {
    // interpreter.interpret();
}