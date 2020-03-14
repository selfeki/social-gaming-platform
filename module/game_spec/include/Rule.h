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

struct Rule {
    virtual ~Rule() = default;

    virtual void accept(RuleVisitor& visitor) const = 0;
};

using RuleID = int;
using RuleList = std::vector<std::unique_ptr<Rule>>;

struct ForEach final : public Rule {
    virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }

    // raw pointer is fine because it is non-owning
    Rule* parent;
    Expression elemList;
    Expression elem;
    RuleList rules;
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

struct Add  : public Rule{
    virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }
    //int id;
     Rule*		 parent;
     Expression to;
     Expression value;
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

struct InputText : public Rule {
    virtual void accept(RuleVisitor &visitor) const { return visitor.visit(*this); }

//     int id;
    Rule *parent;
//     UserID 		 targetUser;
    Expression prompt;
    Expression result;
    Expression timeout;
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
    virtual void accept(RuleVisitor& visitor) const { return visitor.visit(*this); }

    Rule* parent;
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
