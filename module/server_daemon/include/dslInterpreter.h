#ifndef DSLINTERPRETER_H
#define DSLINTERPRETER_H 

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


std::map<RuleType, std::string> RuleTypeMap;
RuleTypeMap["for each"] = RuleType.FOR_EACH;
RuleTypeMap["loop"] = RuleType.LOOP;
RuleTypeMap["inparallel"] = RuleType.IN_PARALLEL;


class State;

class Node;

class Node {
	public:
		Node();
		Node(Node& parent);
		~Node();
	private:
		Node* parent;
};

class ElementNode: public Node{
	public:
		ElementNode();
		ElementNode(std::string data ){data=data;}
		ElementNode(Node& parent, std::string data) 
			:Node{parent},
				data{data}
			{}
		~ElementNode();
		std::get_data() {return data ;} ;
	private:
		std::string data ;
};

class ListNode: public Node{
	public:
		ListNde();
		ListNode(Node& parent)
			:Node{parent} 
			{}
		~ListNode();
		std::vector<Node> getChildren(){return children};
		void addChild(Node node) ;
	private:
		std::vector<Node> children ;
};


class RuleNode : public LsitNode{
public:
	RuleNode(RuleType ruleType);
	RuleNode(Node& parent, RuleType ruleType)
		:Node{parent},ls
			type{ruleType}
			{}
	~RuleNode();
	Node getType(){return type}; 
private:
	RuleType type;
};


class DSLInterpreter {
public:
	State getNewState(State& state);

private:
	RuleNode parseRule(SpecWrapper Spec);
  interpretRule()
  interpretGlobalMessage(Rule)
  State state;
	std::vector<RuleNode> Rules ;
};

#endif 