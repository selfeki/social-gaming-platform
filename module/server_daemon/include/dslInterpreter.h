#ifndef DSLInterpreter
#define DSLInterpreter 

#include <vector>


enum RuleKeyWord {
  FOR_EACH,
  LOOP,
  IN_PARALLEL,
  PARALLEL_FOR,
  SWITCH,
  WHEN,
  EXTEND,
  REVERSE,
  SHUFFLE,
  SORT,
  DEAL,
  DISCARD,
  ADD,
  TIMER,
  INPUT_CHOICE,
  INPUT_TEXT,
  INPUT_VOTE,
  MESSAGE,
  GLOBAL_MESSAGE,
  SCORES
};


class State;


class Rule {
public:

private:
  std::vector<Rule> children;

};


class DSLInterpreter {
public:
	State getNewState(State& state);

private:
  interpretRule()
  interpretGlobalMessage(Rule)
  State state;
};

#endif 