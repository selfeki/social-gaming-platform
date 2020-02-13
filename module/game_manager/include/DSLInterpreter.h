#include <GameState.h>
#include <map>
#include <optional>
#include <string>
#include <vector>


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

// todo: replace this with equivalent boost::variant implementation


class Node {
public:
    Node()
        : parent { std::nullopt } {}

    Node(Node* parent)
        : parent { parent } {}

    std::optional<Node*>&
    getParent() { return parent; }

    void
    setParent(Node& node) { parent = &node; }

private:
    std::optional<Node*> parent;
};


class RuleNode : public Node {
public:
    RuleNode(Node& parent, Node& child, RuleType ruleType)
        : Node(parent)
        , child { &child }
        , type { ruleType } {}

    const Node&
    getChild() { return *child; }

    void
    setChild(Node& node) { child = &node; }

    RuleType getType() { return type; }

private:
    Node* child;

    RuleType type;
};


class ListNode : public Node {
public:
    ListNode(Node& parent, std::vector<Node>& ch)
        : Node(parent)
        , children { ch } {}

    const std::vector<Node>&
    getChildren() { return children; }

    void
    addChild(Node& node) { children.push_back(node); }

    // TODO
    void
    RemoveChild(Node& node);

private:
    std::vector<Node>& children;
};


class ElementNode : public Node {
public:
    ElementNode(Node& parent, std::string& data)
        : Node(parent)
        , data { data } {}

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
        : currentRule { &rule } {}

    void
    setRule(Node rule) { currentRule = &rule; }

    GameState
    interpret(Environment& env, GameState& state);

private:
    Node* currentRule;
};