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
{{}, {}, {P_LPAREN_T, P_EXPR, P_RPAREN_T, M_PRIMARY}, {}, {P_LITERAL_T, M_PRIMARY}, {}}
};

SyntaxTreeNode::SyntaxTreeNode(NodeType type, char ch) : type(type), value(ch) {}

void SyntaxTreeNode::set_type(NodeType node_type) {
    this->type = node_type;
}

const std::vector<SyntaxTreeNode *> &SyntaxTreeNode::get_children() const {
    return this->children;
}

const std::vector<SyntaxTreeNode> &SyntaxTree::get_nodes() const {
    return this->nodes;
}

SyntaxTreeNode::NodeType SyntaxTreeNode::get_type() const {
    return this->type;
}

void SyntaxTreeNode::insert_child(SyntaxTreeNode *node) {
    this->children.push_back(node);
}

SyntaxTreeNode *SyntaxTree::emplace_node(SyntaxTreeNode::NodeType type, char value) {
    this->nodes.emplace_back(type, value);
    return &this->nodes.back();
}

void SyntaxTree::insert_child(SyntaxTreeNode *father, SyntaxTreeNode *child) {
    father->insert_child(child);
}


Regex::Regex(std::string expr) : expr(std::move(expr)) {
    this->tree = Parser::parse(expr);
}

bool Regex::eval(const std::string &word) {

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

SyntaxTree Parser::parse(const std::string &expr) {
    SyntaxTree tree;

    std::stack<SyntaxTreeNode *> value_stack;
    std::stack<Symbol> prod_stack;
    prod_stack.push(M_END);
    prod_stack.push(P_EXPR);

    auto expr_it = expr.begin();
    Symbol term_sym = Parser::char_to_symbol(*expr_it);

    while(!prod_stack.empty()) {
        Symbol curr_prod = prod_stack.top();
        prod_stack.pop();

        if(curr_prod == P_EPSILON) {
            value_stack.push(nullptr);
            continue;
        }

        if(curr_prod >= P_STAR_T) {
            if(expr_it == expr.end()) break;

            assert(curr_prod == term_sym);

            expr_it++;
            if(expr_it == expr.end()) {
                term_sym = EOF_T;
                continue;
            }
            term_sym = Parser::char_to_symbol(*expr_it);
            continue;
        }
        else if(curr_prod >= M_EXPR) {
            SyntaxTreeNode *node = nullptr;
            switch(curr_prod) {
                case M_EXPR:
                    node = tree.emplace_node(SyntaxTreeNode::OR, '|');
                    if(value_stack.top()) node->insert_child(value_stack.top());
                    value_stack.pop();
                    if(value_stack.top()) node->insert_child(value_stack.top());
                    value_stack.pop();
                    break;
                case M_EXPR_PR:
                    break;
                case M_CONCAT:
                    break;
                case M_CONCAT_PR:
                    break;
                case M_STAR:
                    break;
                case M_STAR_PR:
                    break;
                case M_PRIMARY:
                    break;
                case M_END:
                    break;
                default:
                    break;
            }
        }

        assert(curr_prod < Parser::prod_count);
        assert(term_sym - 8 < Parser::terminal_count);

        std::vector<Symbol> production = Parser::prod_table[curr_prod][term_sym - 8];
        for(auto it = production.end() - 1; !production.empty() && it >= production.begin(); it--) {
            prod_stack.push(*it);
        }
    }

    return tree;
}
