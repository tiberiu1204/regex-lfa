#include "regex_engine.h"

#include <utility>
#include <stack>

SyntaxTree::Node::Node(SyntaxTree::NodeType type) : type(type) {}
void SyntaxTree::Node::set_type(SyntaxTree::NodeType node_type) {
    this->type = node_type;
}

SyntaxTree::NodeType SyntaxTree::Node::get_type() const {
    return this->type;
}

void SyntaxTree::Node::insert_child(Node *node) {
    this->children.push_back(node);
}

void SyntaxTree::emplace_node(SyntaxTree::NodeType type) {
    this->nodes.emplace_back(type);
}

Regex::Regex(std::string expr) : expr(std::move(expr)) {
    this->tree = Parser::parse(expr);
}

bool Regex::eval(const std::string &word) {

}
/*
 * GRAMMAR:
 * expr :- concat expr' $ ;
 * expr' :- '|' concat expr' | epsilon ;
 * concat :- primary concat' | star concat' ;
 * concat' :- primary concat' | star concat' | epsilon ;
 * star :- primary '*' ;
 * primary :- literal | '(' expr ')' ;
 *
 * FIRST(expr) = { literal, ( }
 * FIRST(expr') = { |, epsilon }
 * FIRST(concat) = { literal, ( }
 * FIRST(concat') = { literal, (, epsilon }
 * FIRST(STAR) = { literal, ( }
 * FIRST(primary) = { literal, ( }
 *
 * FORWARD(expr) = { ), $ }
 * FORWARD(expr') = { ), $ }
 * FORWARD(concat) = { |, ), $ }
 * FORWARD(concat') = { |, ), $ }
 * FORWARD(star) = { literal, (, |, ), $ }
 * FORWARD(primary) = { *, literal, (, |, ), $ }
 *
 */

SyntaxTree Parser::parse(const std::string &expr) {

}
