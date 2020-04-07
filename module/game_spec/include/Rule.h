#pragma once

#include "GameState.h"
#include "Expression.h"
#include "MapWrapper.h"

#include <memory>

namespace gameSpecification::rule {


struct Rule;
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

    void visit(ForEach& rule)       { visitImpl(rule); }
    void visit(GlobalMessage& rule) { visitImpl(rule); }
    void visit(ParallelFor& rule)   { visitImpl(rule); }
    void visit(InputChoice& rule)   { visitImpl(rule); }
    void visit(Discard& rule)       { visitImpl(rule); }
    void visit(Reverse& rule)       { visitImpl(rule); }
    void visit(Extend& rule)        { visitImpl(rule); }
    void visit(InParallel& rule)    { visitImpl(rule); }
    void visit(Add& rule)           { visitImpl(rule); }
    void visit(Shuffle& rule)       { visitImpl(rule); }
    void visit(Message& rule)       { visitImpl(rule); }
    void visit(Sort& rule)          { visitImpl(rule); }
    void visit(Deal& rule)          { visitImpl(rule); }
    void visit(Scores& rule)        { visitImpl(rule); }
    void visit(InputText& rule)     { visitImpl(rule); }
    void visit(InputVote& rule)     { visitImpl(rule); }
    void visit(Loop& rule)          { visitImpl(rule); }
    void visit(Switch& rule)        { visitImpl(rule); }
    void visit(Timer& rule)         { visitImpl(rule); }
    void visit(When& rule)          { visitImpl(rule); }

private:
    virtual void visitImpl(ForEach& rule) {}
    virtual void visitImpl(GlobalMessage& rule) {}
    virtual void visitImpl(ParallelFor& rule) {}
    virtual void visitImpl(InputChoice& rule) {}
    virtual void visitImpl(Discard& rule) {}
    virtual void visitImpl(Reverse& rule) {}
    virtual void visitImpl(Extend& rule) {}
    virtual void visitImpl(InParallel& rule) {}
    virtual void visitImpl(Add& rule) {}
    virtual void visitImpl(Shuffle& rule) {}
    virtual void visitImpl(Message& rule) {}
    virtual void visitImpl(Sort& rule) {}
    virtual void visitImpl(Deal& rule) {}
    virtual void visitImpl(Scores& rule) {}
    virtual void visitImpl(InputText& rule) {}
    virtual void visitImpl(InputVote& rule) {}
    virtual void visitImpl(Loop& rule) {}
    virtual void visitImpl(Switch& rule) {}
    virtual void visitImpl(Timer& rule) {}
    virtual void visitImpl(When& rule) {}
};


using RulePtr = std::unique_ptr<Rule>;

struct Rule {
    virtual void accept(RuleVisitor& visitor) = 0;

    Rule() {};
    virtual ~Rule() = default;

    // raw pointer okay b/c child does not own parent
    Rule*   parent;
    RulePtr next;
    RulePtr nested;
    
};


struct ForEach final : public Rule {
    virtual void accept(RuleVisitor& visitor) { visitor.visit(*this); }

    ForEach(Expression _elemList, Expression _elem)
        : elemList(_elemList)
        , elem(_elem)
        , elemListSize(castExp<ExpList>(_elemList).getSize()) {}

    Expression  elemList;
    Expression  elem;
    int elemListSize;
};


using RuleList = std::vector<RulePtr>;

struct ParallelFor final : public Rule {
    virtual void accept(RuleVisitor& visitor) { return visitor.visit(*this); }

    Expression  elemList;
    Expression  elem;
};


struct Discard : public Rule {
    virtual void accept(RuleVisitor& visitor) { return visitor.visit(*this); }

    Discard(Expression _fromList, Expression _count)
        : fromList(_fromList),
          count(_count) {}

    Expression fromList;
    Expression count;
};


struct Reverse : public Rule {
    virtual void accept(RuleVisitor& visitor) { return visitor.visit(*this); }

    Reverse(Expression _list)
        : list(_list) {}
    Expression list;

};


struct Extend : public Rule {
    virtual void accept(RuleVisitor& visitor) { return visitor.visit(*this); }

    Expression targetList;
    Expression list;
};


struct Shuffle : public Rule {
    virtual void accept(RuleVisitor& visitor) { return visitor.visit(*this); }

    Shuffle(Expression _list)
        : list(_list) {}

    Expression list;
};


enum LoopType {
    UNTIL,
    WHILE
};


struct Loop : public Rule {
    virtual void accept(RuleVisitor& visitor) { return visitor.visit(*this); }

    LoopType type;
};


struct InParallel : public Rule {
    virtual void accept(RuleVisitor& visitor) { return visitor.visit(*this); }

};


struct Message : public Rule {
    virtual void accept(RuleVisitor& visitor) { return visitor.visit(*this); }

    Expression targetUsers;
    Expression content;
};


/*
Notes:
    possibel element values
    a variable "count" which stores a number
    count.upfrom(1)
    expresses a list of numbers [1, 2, … , count]

    list attributes
    "roles.size"

    Only if the elementLists are maps ->

    "roles.elements.name" defines the list of names contained within the above list.
    Additional useful attributes are
    "contains" and "collect"
*/


struct Deal : public Rule {
    virtual void accept(RuleVisitor& visitor) { return visitor.visit(*this); }

    Expression fromList;
    Expression toList;
    Expression count;
};


struct Add final : public Rule {
    Add(Expression _to, Expression _value)
        : to(_to)
        , value(_value) {}

    virtual void accept(RuleVisitor& visitor) { visitor.visit(*this); }

    Expression to;
    Expression value;
};


using CaseToRules = MapWrapper<Expression, RulePtr>;

struct Switch : public Rule {
    virtual void accept(RuleVisitor& visitor) { return visitor.visit(*this); }

    Expression  switchTarget;
    Expression  valuesList;
    RuleList    rules;
    CaseToRules caseToRules;
};


using ConditionToRules = MapWrapper<Expression, RulePtr>;

struct When : public Rule {
    virtual void accept(RuleVisitor& visitor) { return visitor.visit(*this); }

    ConditionToRules condToRules;
};


struct Sort : public Rule {
    virtual void accept(RuleVisitor& visitor) { return visitor.visit(*this); }

    Expression list;
    // todo: if key provided
    // Validate that the list contains maps
    std::optional<Expression> key;
};


struct InputChoice : public Rule {
    virtual void accept(RuleVisitor& visitor) { return visitor.visit(*this); }

    Expression targetUser;
    Expression prompt;
    Expression choiceList;
    Expression result;
    std::optional<Expression> timeout;
};


struct InputText final : public Rule {
    virtual void accept(RuleVisitor& visitor) { visitor.visit(*this); }

    Expression targetUser;
    Expression prompt;
    Expression result;
    // optional
    std::optional<Expression> timeout;
};


struct InputVote : public Rule {
    virtual void accept(RuleVisitor& visitor) { return visitor.visit(*this); }

    Expression targetUsers;
    Expression prompt;
    Expression choiceList;
    Expression resultMap;
    std::optional<Expression> timeout;
};


enum TimerMode {
    EXACT,
    AT_MOST,
    TRACK
};

struct Timer : public Rule {
    virtual void accept(RuleVisitor& visitor) { return visitor.visit(*this); }

    Expression  duration;
    TimerMode   mode;
    std::optional<Expression> flag;
};


struct GlobalMessage final : public Rule {
    virtual void accept(RuleVisitor& visitor) { visitor.visit(*this); }

    std::string content;
};


struct Scores final : public Rule {
    virtual void accept(RuleVisitor& visitor) { return visitor.visit(*this); }

    Expression scoreAttribute;
    Expression isAscending;
};

}    // namespace rule