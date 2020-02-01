#include "../include/dslIntrpreter.h"

#include <vector>

Node::Node(){
    parent=NULL ;
}
Node::Node(Node& parent){
    parent=parent;
}
Node::~Node(){
    parent = NULL;
    delete parent; 
}

ElementNode::ElementNode(){
    data="No data";
}

RuleNode::RuleNode(RuleType ruleType){
    type=ruleType;
}

void ListNode::addChild(Node node){
    children.push_back(node) ;
}

RuleNode DSLInterpreter::parseRule(SpecWrapper spec){
    RuleNode ruleNode(RuleTypeMap.find(spec.name));

    for(auto child: spec.children){
        if(child.name == "rule"){
            ruleNode.addChild(RuleNode(ruleNode, RuleTypeMap.find(child.name))) ;
            praseRule(rule) ;
        }
        else{
            if(child.data){
                ElementNode dataNode = ElementNode(ruleNode, child.data);
                ruleNode.addChild(dataNode);
            }
            else{
                ruleNode.addChild(Node(ruleNode));
            }
        }
    }
    return ruleNode ;
}