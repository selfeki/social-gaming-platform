#include <vector>
#include <string>
#include <map>

#include <State.h>


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

// this does not compile
// std::map<RuleType, std::string> RuleTypeMap;
// RuleTypeMap["for each"] = RuleType.FOR_EACH;
// RuleTypeMap["loop"] = RuleType.LOOP;
// RuleTypeMap["inparallel"] = RuleType.IN_PARALLEL;


class Node {
public:
  Node(Node& parent)
    : parent{&parent}
      { }

  const Node& 
  getParent() { return *parent; }

  void 
  setParent(Node& node) { parent = &node; }

private:
  Node* parent;
};


class RuleNode : public Node {
public:
	RuleNode(Node& parent, Node& child, RuleType ruleType);

  const Node& 
  getChild() { return *child; }

  void 
  setChild(Node& node) { child = &node; }

	RuleType getType() { return type; }

private:
  Node* child;

	RuleType type;
};


class ListNode: public Node {
	public:
		ListNode(Node& parent, std::vector<Node>& children);

		const std::vector<Node>&
    getChildren() { return children; }

		void 
    addChild(Node& node) { children.emplace_back(node); }

    // TODO
    void
    RemoveChild(Node& node);

	private:
		std::vector<Node>& children;
};


class ElementNode: public Node {
public:
  ElementNode(Node& parent, std::string& data)
    : Node(parent),
      data{data}
      { }

  const std::string&
  getData() { return data; }

private:
  std::string& data;
};


// todo: separate rule parsing and interpreting into different components
// todo: identify end of interpretation inside interpretVisitor

class DSLInterpreter {
public:
  DSLInterpreter(Node& rule)
    : currentRule{&rule}
      { }
  
  void
  setRule(Node rule) { currentRule = &rule; }

  State
  interpret(Environment& env, State& state);

private:
  State
  interpretRule(State state, Node& rule);

  Node* currentRule;
};