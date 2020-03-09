#pragma once

#ifndef RULE_H
#define RULE_H

#include "Expression.h"
#include "MapWrapper.h"

// #include <memory>

namespace gameSpecification::rule {

// Control flow
struct ForEach;
struct Loop;
struct InParallel;
struct ParallelFor;
struct Switch;
struct When;
// List ops
struct Extend;
struct Reverse;
struct Shuffle;
struct Sort;
struct Deal;
struct Discard;
// Arithmetic
struct Add;
// Timing
struct Timer;
// Human Input
struct InputChoice;
struct InputText;
struct InputVote;
// Output
struct Message;
struct GlobalMessage;
struct Scores;


class RuleVisitor {
public:
  void visit(const ForEach& forEach) { visitImpl(forEach); }
  virtual ~RuleVisitor();

private:
  virtual void visitImpl(const ForEach& forEach) { }
};


struct Rule {
    virtual void accept(RuleVisitor& visitor) const = 0;
    virtual ~Rule();
};


using RuleList = std::vector<Rule>;

struct ForEach : public Rule {
  virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }

  // std::unique_ptr<Rule> parent;
  Expression   elemList;
  Expression   elem;
  RuleList 	 rules;
};



// enum LoopType {
//     UNTIL,
//     WHILE
// };

// struct Loop : public Rule {
//     int id;
//     Rule& 	 parent;
//     LoopType type;    // Until or While
//     RuleList rules;
// };

// struct InParallel {
//     int id;
//     Rule& 	 parent;
//     RuleList rules;
// };

// struct ParallelFor {
//     int id;
//     Rule& 		 parent;
//     Expression elemList;
//     Expression elem;
//     RuleList 	 rules;
// };

// using CaseToRules = MapWrapper<Expression, RuleList>;

// struct Switch {
//     int id;
//     Rule& 			parent;
//     Expression 	switchTarget;
//     Expression 	valuesList;
//     RuleList 		rules;
//     CaseToRules caseToRules;
// };

// using ConditionToRules = MapWrapper<Expression, RuleList>;

// struct When {
//     int id;
//     Rule& parent;
//     ConditionToRules condToRules;
// };

// struct Extend {
//     int id;
//     Rule& 		 parent;
//     Expression targetList;
//     Expression list;
// };

// struct Reverse {
//     int id;
//     Rule& 		 parent;
//     Expression list;
// };

// struct Shuffle {
//     int id;
//     Rule& 		 parent;
//     Expression list;
// };

// struct Sort {
//     int id;
//     Rule& 		 parent;
//     Expression list;
//     // todo: if key provided
//     // Validate that the list contains maps
//     std::optional<std::string> key;
// };

// // possibel element values
// // a variable "count" which stores a number
// // count.upfrom(1)
// // expresses a list of numbers [1, 2, … , count]

// // list attributes
// // "roles.size"

// // Only if the elementLists are maps ->

// // "roles.elements.name" defines the list of names contained within the above list.
// // Additional useful attributes are
// // "contains" and "collect"

// // Number is an integer literal?
// struct Deal {
//     int id;
//     Rule& 		 parent;
//     Expression fromList;
//     Expression toList;
//     Expression count;
// };

// struct Discard {
//     int id;
//     Rule& 		 parent;
//     Expression fromList;
//     Expression count;
// };

// struct Add {
//     int id;
//     Rule& 		 parent;
//     Expression to;
//     Expression value;
// };

// enum TimerMode {
//     EXACT,
//     AT_MOST,
//     TRACK
// };

// struct Timer {
//     int id;
//     Rule& 		 parent;
//     Expression duration;
//     TimerMode  mode;
//     RuleList 	 rules;
//     Expression flag;
// };

// // todo: make consistent with GameManager user ID?
// using UserID = std::string;
// using UserIDList = std::vector<UserID>;


// struct InputChoice {
//     int id;
//     Rule& 		 parent;
//     UserIDList targetUsers;
//     Expression prompt;
//     Expression choiceList;
//     Expression result;
//     std::optional<Expression> timeout;
// };

// struct InputText {
//     int id;
//     Rule& 		 parent;
//     UserID 		 targetUser;
//     Expression prompt;
//     Expression result;
//     // optional
//     std::optional<Expression> timeout;
// };

// struct InputVote {
//     int id;
//     Rule& 		 parent;
//     UserIDList targetUsers;
//     Expression prompt;
//     Expression choiceList;
//     Expression resultMap;
// };

// struct Message {
//     int id;
//     Rule& 		 parent;
//     UserIDList targetUsers;
//     Expression content;
// };

// struct GlobalMessage {
//     int id;
//     Rule& 		 parent;
//     Expression content;
// };

// struct Scores {
//     int id;
//     Rule&      parent;
//     Expression scoreAttribute;
//     Expression isAscending;
// };

}    // namespace rule

#endif /* RULE_H */
