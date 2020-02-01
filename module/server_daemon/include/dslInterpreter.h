#ifndef DSLInterpreter
#define DSLInterpreter 

#include <vector>
#include <string.h>


enum RuleType {
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

class Node;

class Node {
	public:
		Node(std::string name, std::vector<Node> children) ;
		std::string getName();
		Node getParent();
		std::vector<Node> getChildren() ;
	private:
		std::string name;
		Node parent;
		std::vector<Node> children;
};


class RuleNode : public Node {
public:
	RuleNode(std:: string element, std::vector<RuleNode> children);
	std::string getElement(); 
	std::vector<RuleNode> getChildren();

private:
	RuleType type;
  std::vector<RuleNode> children;

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