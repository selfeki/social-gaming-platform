#include <vector>
#include <string>
#include <optional>
#include <boost/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>

#include "MapWrapper.h"


namespace rules {

// Control flow
struct ForEach;
struct Loop;
struct InParallel;
struct ParallelFor;
struct Switch;
struct When;
// ElemLista ops
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


using Map  = MapWrapper<std::string, boost::recursive_variant_>;
using List = std::vector<boost::recursive_variant_>;

using Expression = boost::make_recursive_variant<
  Map,
  List,
  std::string,
  int,
  bool>::type;


using RuleList = std::vector<Rule>;

struct ForEach {
  // ExpressionList itself should be a variant of
  // <<list, list expression, or listName>>
  // Difference b/w list and list expression?
  Expression  elemList; 
  Expression  elem;
  RuleList    rules;
}

struct Loop {
  LoopType type; // Until or While
  RuleList rules;
}

struct Inparallel {
  RuleList rules;
}

struct ParallelFor {
  Expression  elemList;
  Expression  elem;
  RuleList    rules;
}

using CaseToRules = MapWrapper<Expression, Rules>;

struct Switch {
  Expression  switchTarget;
  Expression  valuesList
  RuleList    rules;
  CaseToRules caseToRules;
}

using ConditionToRules = MapWrapper<Expression, Rules>;

struct When {
  ConditionToRules condToRules;
}

struct Extend {
  Expression targetList;
  Expression list;
}

struct Reverse {
  Expression list;
}

struct Shuffle {
  Expression list;
}

struct Sort {
  Expression list;
  // todo: if key provided
  // Validate that the list contains maps
  std::optional<std::string> key;
}

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
  Expression  fromList;
  Expression  toList;
  Number      count;
}

struct Discard {
  Expression  fromList;
  Number      count;
}

struct Add {
  Expression to;
  Expression value;
}

enum TimerMode {
  EXACT,
  AT_MOST,
  TRACK
};

struct Timer {
  Number    duration;
  TimerMode mode;
  RuleList  rules
  int       flag;
}

using UserID = std::string;
using Message = std::string;

struct InputText {
  UserID      targetUser;
  Message     prompt;
  Expression  result;
  // optional
  std::optional<Number> timeout;
}

using UserIDList = std::vector<UserID>;

struct InputVote {
  UserIDList  targetUsers;
  Message     prompt;
  Expression     choiceList;
  Expression     resultMap;
}

// todo: finish the rest

} // namespace rules