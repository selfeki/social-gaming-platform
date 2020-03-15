#pragma once

#ifndef RULE_H
    #define RULE_H

    #include "Expression.h"
    #include "MapWrapper.h"
    #include <memory>

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
    virtual ~RuleVisitor() = default;

    void visit(const ForEach& rule) { visitImpl(rule); }
    void visit(const GlobalMessage& rule) { visitImpl(rule); }
    void visit(const ParallelFor& rule) { visitImpl(rule); }
    void visit(const InputChoice& rule) { visitImpl(rule); }
    void visit(const Discard& rule) { visitImpl(rule); }
    void visit(const Reverse& rule) { visitImpl(rule); }
    void visit(const Extend& rule) { visitImpl(rule); }
    void visit(const InParallel& rule) { visitImpl(rule); }
    void visit(const Add& rule) { visitImpl(rule); }
    void visit(const Shuffle& rule) { visitImpl(rule); }
    void visit(const Message& rule) { visitImpl(rule); }
    void visit(const Sort& rule) { visitImpl(rule); }
    void visit(const Deal& rule) { visitImpl(rule); }
    void visit(const Scores& rule) { visitImpl(rule); }
    void visit(const InputText& rule) { visitImpl(rule); }
    void visit(const InputVote& rule) { visitImpl(rule); }
    void visit(const Loop& rule) { visitImpl(rule); }
    void visit(const Switch& rule) { visitImpl(rule); }
    void visit(const Timer& rule) { visitImpl(rule); }
    void visit(const When& rule) { visitImpl(rule); }

private:
    virtual void visitImpl(const ForEach& rule) {}
    virtual void visitImpl(const GlobalMessage& rule) {}
    virtual void visitImpl(const ParallelFor& rule) {}
    virtual void visitImpl(const InputChoice& rule) {}
    virtual void visitImpl(const Discard& rule) {}
    virtual void visitImpl(const Reverse& rule) {}
    virtual void visitImpl(const Extend& rule) {}
    virtual void visitImpl(const InParallel& rule) {}
    virtual void visitImpl(const Add& rule) {}
    virtual void visitImpl(const Shuffle& rule) {}
    virtual void visitImpl(const Message& rule) {}
    virtual void visitImpl(const Sort& rule) {}
    virtual void visitImpl(const Deal& rule) {}
    virtual void visitImpl(const Scores& rule) {}
    virtual void visitImpl(const InputText& rule) {}
    virtual void visitImpl(const InputVote& rule) {}
    virtual void visitImpl(const Loop& rule) {}
    virtual void visitImpl(const Switch& rule) {}
    virtual void visitImpl(const Timer& rule) {}
    virtual void visitImpl(const When& rule) {}
};

using Name = std::string_view;
using PlayerMessage = std::pair<Name, std::string>;



struct Rule {

    Rule() : finished(false), needsInput(false), nestedRulesInProgess(false) {};
    virtual ~Rule() = default;
    virtual void accept(RuleVisitor& visitor) = 0;
    
    //returns messages to be sent, then deletes them from the rule
    virtual std::vector<PlayerMessage> popOutGoingMessages() = 0;

    bool finished;
    bool needsInput;
    bool nestedRulesInProgess;
    Rule* next;
    Rule* next_nested;
    Rule* parent;

};

using RuleID = int;
using RuleList = std::vector<std::unique_ptr<Rule>>;


struct ForEach final : public Rule {

    ForEach(Expression _elemList, Expression _elem) : elemList(_elemList), elem(_elem) {
        elemListIndex = 0;
        elemListSize = (boost::get<ExpList>(_elemList)).list.size(); //ugly
    }

    virtual void accept(RuleVisitor& visitor)  { visitor.visit(*this); }
    virtual std::vector<PlayerMessage> popOutGoingMessages() {return {};}

    // raw pointer is fine because it is non-owning
    Expression elemList;
    Expression elem;
    int elemListSize;
    int elemListIndex;
};

struct ParallelFor final : public Rule {
    virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }
     //int id;
     Rule* 		 parent;
     Expression elemList;
     Expression elem;
     RuleList 	 rules;
};

struct InputChoice  : public Rule {
    virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }
        //int id;
        Rule* parent;
        //TODO: determine how to store list of users
        //UserIDList targetUsers;
        Expression prompt;
        Expression choiceList;
        Expression result;
        std::optional<Expression> timeout;
};

struct Discard : public Rule {
    virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }
    //int id;
        Rule* 		 parent;
        Expression fromList;
        Expression count;
};

struct Reverse : public Rule{
     virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }
     //int id;
     Rule* 		 parent;
     Expression list;
};

struct Extend : public Rule{
    virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }
    //int id;
     Rule*		parent;
     Expression targetList;
     Expression list;
};

struct Shuffle : public Rule{
    virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }
    //     int id;
    Rule* 		 parent;
    Expression list;
};


enum LoopType {
     UNTIL,
     WHILE
};

struct Loop : public Rule {
    virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }

//     int id;
    Rule* 	 parent;
    LoopType type;    // Until or While
    RuleList rules;
};

struct InParallel : public Rule{
    virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }
    //int id;
     Rule*	 parent;
     RuleList rules;
};



struct Message : public Rule{
    virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }
    //int id;
    Rule* 		 parent;
    //UserIDList targetUsers;
    Expression content;
};

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
struct Deal : public Rule{
    virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }
//     int id;
     Rule* 		 parent;
     Expression fromList;
     Expression toList;
     Expression count;
};

using CaseToRules = MapWrapper<Expression, RuleList>;

 struct Add final : public Rule {
    Add(Expression _to, Expression _value) : to(_to), value(_value) {}

    virtual void accept(RuleVisitor& visitor)  { visitor.visit(*this); }
    virtual std::vector<PlayerMessage> popOutGoingMessages() {return {};}

     Expression to;
     Expression value;
 };

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
 struct Switch : public Rule{
     virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }
//     int id;
     Rule*			parent;
     Expression 	switchTarget;
     Expression 	valuesList;
     RuleList 		rules;
     CaseToRules caseToRules;
};

using ConditionToRules = MapWrapper<Expression, RuleList>;

struct When : public Rule{
    virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }
//     int id;
     Rule* parent;
     ConditionToRules condToRules;
};


struct Sort : public Rule{
    virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }
//     int id;
     Rule* 		 parent;
     Expression list;
     // todo: if key provided
     // Validate that the list contains maps
     //std::optional<std::string> key;
     Expression key;
};

//     Rule& 		 parent;
//     UserIDList targetUsers;
//     Expression prompt;
//     Expression choiceList;
//     Expression result;
//     std::optional<Expression> timeout;
// };

struct InputText final : public Rule {
    InputText(Expression _to, Expression _prompt, Expression _result) 
    : to(_to), prompt(_prompt), result(_result) {}

    virtual void accept(RuleVisitor& visitor)  { visitor.visit(*this); }
    virtual std::vector<PlayerMessage> popOutGoingMessages() {return {};}

    Expression 	 to;
    Expression   prompt;
    Expression   result;
    // optional
    std::optional<Expression> timeout;
};


struct InputVote : public Rule {
     virtual void accept(RuleVisitor &visitor) const { return visitor.visit(*this); }
//     int id;
    Rule* 		 parent;
     //UserIDList targetUsers;
     Expression prompt;
     Expression choiceList;
     Expression resultMap;
     //std::optional<Expression> timeout;
     Expression timeout;
};



enum TimerMode {
     EXACT,
     AT_MOST,
     TRACK
};

struct Timer : public Rule {
    virtual void accept(RuleVisitor &visitor) const { return visitor.visit(*this); }
     //int id;
     Rule*		 parent;
     Expression duration;
     TimerMode  mode;
     RuleList 	 rules;
     Expression flag;
};

struct GlobalMessage final : public Rule {
    GlobalMessage(Expression _content) 
    : content(_content) {}

    virtual void accept(RuleVisitor& visitor)  { visitor.visit(*this); }
    virtual std::vector<PlayerMessage> popOutGoingMessages() {return {};}

    Expression content;
};

struct Scores final : public Rule {
     virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }
//     int id;
     Rule*      parent;
     Expression scoreAttribute;
     Expression isAscending;
};

}    // namespace rule

#endif /* RULE_H */
