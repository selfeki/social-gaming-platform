#include <vector>
#include <string>
#include <DSLInterpreter.h>

// TODO either implement Visitor or change Node class to std::variant
// class NodeVisitor;

// class InterpretVisitor : public NodeVisitor {
// public:
//   void
//   visit(const RuleNode& rule) {
      
//   }

//   void
//   visit(const ListNode& list) {
      
//   }

//   void
//   visit(const ElementNode& elem) {
      
//   }

// private:
//   State& state;
//   RuleNode& currentRule;
// };


  // interprets current rule
  // returns new state after interpretation
  State 
  DSLInterpreter::interpret(Environment& env, State& state) {
    // TODO
  }