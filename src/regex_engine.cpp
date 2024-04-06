#include "regex_engine.h"
#include <utility>
#include <stack>
#include <cassert>
#include <iostream>

std::vector<Parser::Symbol> Parser::prod_table[prod_count][terminal_count] = {
{{}, {}, {P_CONCAT, P_EXPR_PR, M_EXPR}, {}, {P_CONCAT, P_EXPR_PR, M_EXPR}, {}},
{{}, {P_OR_T, P_CONCAT, P_EXPR_PR, M_EXPR_PR}, {}, {P_EPSILON}, {}, {P_EPSILON}},
{{}, {}, {P_STAR, P_CONCAT_PR, M_CONCAT}, {}, {P_STAR, P_CONCAT_PR, M_CONCAT}, {}},
{{}, {P_EPSILON}, {P_STAR, P_CONCAT_PR, M_CONCAT_PR}, {P_EPSILON}, {P_STAR, P_CONCAT_PR, M_CONCAT_PR}, {P_EPSILON}},
{{}, {}, {P_PRIMARY, P_STAR_PR, M_STAR}, {}, {P_PRIMARY, P_STAR_PR, M_STAR}, {}},
{{P_STAR_T, M_STAR_PR}, {P_EPSILON}, {P_EPSILON}, {P_EPSILON}, {P_EPSILON}, {P_EPSILON}},
{{}, {}, {P_LPAREN_T, P_EXPR, P_RPAREN_T}, {}, {P_LITERAL_T}, {}}
};

SyntaxTreeNode::SyntaxTreeNode(NodeType type, char ch) : type(type), value(ch) {}

void SyntaxTreeNode::set_type(NodeType node_type) {
    this->type = node_type;
}

const std::vector<int> &SyntaxTreeNode::get_children() const {
    return this->children;
}

const std::vector<SyntaxTreeNode> &SyntaxTree::get_nodes() const {
    return this->nodes;
}

SyntaxTreeNode::NodeType SyntaxTreeNode::get_type() const {
    return this->type;
}

void SyntaxTreeNode::insert_child(int node_index) {
    this->children.push_back(node_index);
}

int SyntaxTree::emplace_node(SyntaxTreeNode::NodeType type, char value) {
    this->nodes.emplace_back(type, value);
    return static_cast<int>(this->nodes.size() - 1);
}

Regex::Regex(std::string expr) : expr(std::move(expr)) {
    this->tree = Parser::parse(expr);
}

bool Regex::eval(const std::string &word) {
    return false;
}

Parser::Symbol Parser::char_to_symbol(char ch) {
    switch(ch) {
        case '*':
            return P_STAR_T;
        case '|':
            return P_OR_T;
        case '(':
            return P_LPAREN_T;
        case ')':
            return P_RPAREN_T;
        default:
            return P_LITERAL_T;
    }
}

void SyntaxTree::insert_child(int father_index, int child_index) {
    this->nodes[father_index].insert_child(child_index);
}

SyntaxTree Parser::parse(const std::string &expr) {
    SyntaxTree tree;

    std::stack<int> value_stack;
    std::stack<Symbol> prod_stack;
    prod_stack.push(M_END);
    prod_stack.push(P_EXPR);

    auto expr_it = expr.begin();
    Symbol term_sym = Parser::char_to_symbol(*expr_it);

    while(!prod_stack.empty()) {
        Symbol curr_prod = prod_stack.top();
        prod_stack.pop();

        if(curr_prod == P_EPSILON) {
            value_stack.push(-1);
            continue;
        }

        if(curr_prod >= P_STAR_T) {
            if(expr_it == expr.end()) break;

            assert(curr_prod == term_sym);

            if(term_sym == P_LITERAL_T) {
                int node_index = tree.emplace_node(SyntaxTreeNode::LITERAL, *expr_it);
                value_stack.push(node_index);
            }

            expr_it++;
            if(expr_it == expr.end()) {
                term_sym = EOF_T;
                continue;
            }
            term_sym = Parser::char_to_symbol(*expr_it);
            continue;
        }
        else if(curr_prod >= M_EXPR) {
            int node = -1, child1, child2;
            switch(curr_prod) {
                case M_EXPR:
                case M_EXPR_PR:
                    child1 = value_stack.top();
                    value_stack.pop();
                    child2 = value_stack.top();
                    value_stack.pop();
                    if(child1 < 0 && child2 < 0) break;
                    node = tree.emplace_node(SyntaxTreeNode::OR, 0);
                    if(child1 >= 0) tree.insert_child(node, child1);
                    if(child2 >= 0) tree.insert_child(node, child2);
                    break;
                case M_CONCAT:
                case M_CONCAT_PR:
                    child1 = value_stack.top();
                    value_stack.pop();
                    child2 = value_stack.top();
                    value_stack.pop();
                    if(child1 < 0 && child2 < 0) break;
                    node = tree.emplace_node(SyntaxTreeNode::CONCAT, 0);
                    if(child1 >= 0) tree.insert_child(node, child1);
                    if(child2 >= 0) tree.insert_child(node, child2);
                    break;
                case M_STAR:
                    child1 = value_stack.top();
                    value_stack.pop();
                    if(child1 < 0) break;
                    node = tree.emplace_node(SyntaxTreeNode::STAR, 0);
                    tree.insert_child(node, child1);
                    break;
                case M_END:
                    break;
                default:
                    break;
            }
            if(node >= 0) value_stack.push(node);
            continue;
        }

        assert(curr_prod < Parser::prod_count);
        assert(term_sym - P_STAR_T < Parser::terminal_count);

        std::vector<Symbol> production = Parser::prod_table[curr_prod][term_sym - P_STAR_T];
        for(auto it = production.end() - 1; !production.empty() && it >= production.begin(); it--) {
            prod_stack.push(*it);
        }
    }

    return tree;
}
