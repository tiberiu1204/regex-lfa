#include "regex_engine.h"
#include <utility>
#include <stack>
#include <cassert>
#include <iostream>

std::vector<Parser::Symbol> Parser::prod_table[prod_count][terminal_count] = {
{{}, {}, {P_CONCAT, P_EXPR_PR}, {}, {P_CONCAT, P_EXPR_PR}, {}},
{{}, {P_OR_T, P_CONCAT, P_EXPR_PR}, {}, {P_EPSILON}, {}, {P_EPSILON}},
{{}, {}, {P_STAR, P_CONCAT_PR}, {}, {P_STAR, P_CONCAT_PR}, {}},
{{}, {P_EPSILON}, {P_STAR, P_CONCAT_PR}, {P_EPSILON}, {P_STAR, P_CONCAT_PR}, {P_EPSILON}},
{{}, {}, {P_PRIMARY, P_STAR_PR}, {}, {P_PRIMARY, P_STAR_PR}, {}},
{{P_STAR_T}, {P_EPSILON}, {P_EPSILON}, {P_EPSILON}, {P_EPSILON}, {P_EPSILON}},
{{}, {}, {P_LPAREN_T, P_EXPR, P_RPAREN_T}, {}, {P_LITERAL_T}, {}}
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

void SyntaxTree::emplace_node(SyntaxTreeNode::NodeType type, char value) {
    this->nodes.emplace_back(type, value);
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

    std::stack<Symbol> s;
    s.push(P_EXPR);

    auto expr_it = expr.begin();
    Symbol term_sym = Parser::char_to_symbol(*expr_it);

    while(!s.empty()) {
        Symbol curr_prod = s.top();
        s.pop();

        if(curr_prod >= Parser::prod_count) {
            if(curr_prod == P_EPSILON) continue;
            else if(expr_it == expr.end()) break;
            else {
                assert(curr_prod == term_sym);
                if(*expr_it != '*' && *expr_it != '|' && *expr_it != '(' && *expr_it != ')') {
                    tree.emplace_node(SyntaxTreeNode::LITERAL, *expr_it);
                }
                expr_it++;
                if(expr_it == expr.end()) {
                    term_sym = EOF_T;
                    continue;
                }
                term_sym = Parser::char_to_symbol(*expr_it);
                continue;
            }
        }

        assert(curr_prod < Parser::prod_count);
        assert(term_sym - 8 < Parser::terminal_count);

        switch(curr_prod) {
            case P_EXPR:
                tree.emplace_node(SyntaxTreeNode::NodeType::OR, '|');
                break;
            case P_EXPR_PR:
                break;
            case P_CONCAT:
                tree.emplace_node(SyntaxTreeNode::NodeType::CONCAT, '.');
                break;
            case P_CONCAT_PR:
                break;
            case P_STAR:
                tree.emplace_node(SyntaxTreeNode::NodeType::STAR, '*');
                break;
            case P_STAR_PR:
                break;
            default:
                break;
        }

        std::vector<Symbol> production = Parser::prod_table[curr_prod][term_sym - 8];
        for(auto it = production.end() - 1; !production.empty() && it >= production.begin(); it--) {
            s.push(*it);
        }
    }

    return {};
}
