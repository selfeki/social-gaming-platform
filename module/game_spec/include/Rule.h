#pragma once

#include "Expression.h"
#include "MapWrapper.h"

#include <boost/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>

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


using Rule = boost::make_recursive_variant<
    ForEach,
    Loop,
    InParallel,
    ParallelFor,
    Switch,
    When,
    Extend,
    Reverse,
    Shuffle,
    Sort,
    Deal,
    Discard,
    Add,
    Timer,
    InputChoice,
    InputText,
    InputVote,
    Message,
    GlobalMessage,
    Scores>::type;

using RuleList = std::vector<Rule>;

struct ForEach {
    Rule& 		 parent;
    Expression elemList;
    Expression elem;
    RuleList 	 rules;
};

enum LoopType {
    UNTIL,
    WHILE
};

struct Loop {
    Rule& 	 parent;
    LoopType type;    // Until or While
    RuleList rules;
};

struct InParallel {
    Rule& 	 parent;
    RuleList rules;
};

struct ParallelFor {
    Rule& 		 parent;
    Expression elemList;
    Expression elem;
    RuleList 	 rules;
};

using CaseToRules = MapWrapper<Expression, RuleList>;

struct Switch {
    Rule& 			parent;
    Expression 	switchTarget;
    Expression 	valuesList;
    RuleList 		rules;
    CaseToRules caseToRules;
};

using ConditionToRules = MapWrapper<Expression, RuleList>;

struct When {
    Rule& parent;
    ConditionToRules condToRules;
};

struct Extend {
    Rule& 		 parent;
    Expression targetList;
    Expression list;
};

struct Reverse {
    Rule& 		 parent;
    Expression list;
};

struct Shuffle {
    Rule& 		 parent;
    Expression list;
};

struct Sort {
    Rule& 		 parent;
    Expression list;
    // todo: if key provided
    // Validate that the list contains maps
    std::optional<std::string> key;
};

// possibel element values
// a variable "count" which stores a number
// count.upfrom(1)
// expresses a list of numbers [1, 2, … , count]

// list attributes
// "roles.size"

// Only if the elementLists are maps ->

// "roles.elements.name" defines the list of names contained within the above list.
// Additional useful attributes are
// "contains" and "collect"

// Number is an integer literal?
struct Deal {
    Rule& 		 parent;
    Expression fromList;
    Expression toList;
    Expression count;
};

struct Discard {
    Rule& 		 parent;
    Expression fromList;
    Expression count;
};

struct Add {
    Rule& 		 parent;
    Expression to;
    Expression value;
};

enum TimerMode {
    EXACT,
    AT_MOST,
    TRACK
};

struct Timer {
    Rule& 		 parent;
    Expression duration;
    TimerMode  mode;
    RuleList 	 rules;
    Expression flag;
};

// todo: make consistent with GameManager user ID?
using UserID = std::string;
using UserIDList = std::vector<UserID>;


struct InputChoice {
    Rule& 		 parent;
    UserIDList targetUsers;
    Expression prompt;
    Expression choiceList;
    Expression result;
    std::optional<Expression> timeout;
};

struct InputText {
    Rule& 		 parent;
    UserID 		 targetUser;
    Expression prompt;
    Expression result;
    // optional
    std::optional<Expression> timeout;
};

struct InputVote {
    Rule& 		 parent;
    UserIDList targetUsers;
    Expression prompt;
    Expression choiceList;
    Expression resultMap;
};

struct Message {
    Rule& 		 parent;
    UserIDList targetUsers;
    Expression content;
};

struct GlobalMessage {
    Rule& 		 parent;
    Expression content;
};

struct Scores {
    Rule&      parent;
    Expression scoreAttribute;
    Expression isAscending;
};

}    // namespace rule