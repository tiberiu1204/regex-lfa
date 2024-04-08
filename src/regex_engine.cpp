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

int SyntaxTree::root_index() const {
    return static_cast<int>(this->nodes.size() - 1);
}

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
    this->tree = Parser::parse(this->expr);
    this->l_nfa = this->construct_nfa();
}

char SyntaxTreeNode::get_value() const {
    return this->value;
}

Automaton Regex::construct_nfa() {
    struct tree_index {
        int index;
        bool push_automaton;
        tree_index(int index, bool push_automaton) : index(index), push_automaton(push_automaton) {}
    };

    std::stack<tree_index> tree_stack;
    std::stack<Automaton> automaton_stack;
    tree_stack.emplace(this->tree.root_index(), false);

    const std::vector<SyntaxTreeNode> &tree_nodes = this->tree.get_nodes();

    while(!tree_stack.empty()) {
        int node_index = tree_stack.top().index;
        const SyntaxTreeNode &tree_node = tree_nodes[node_index];
        bool push_automaton = tree_stack.top().push_automaton;
        tree_stack.pop();

        if(push_automaton) {
            Automaton automaton, a1, a2;
            switch(tree_node.get_type()) {
                case SyntaxTreeNode::LITERAL:
                    automaton_stack.emplace(tree_node.get_value());
                    break;
                case SyntaxTreeNode::STAR:
                    automaton = *automaton_stack.top();
                    automaton_stack.pop();
                    automaton_stack.push(automaton);
                    break;
                case SyntaxTreeNode::OR:
                    a1 = automaton_stack.top();
                    automaton_stack.pop();
                    a2 = automaton_stack.top();
                    automaton_stack.pop();
                    automaton = a2 | a1;
                    automaton_stack.push(automaton);
                    break;
                case SyntaxTreeNode::CONCAT:
                    a1 = automaton_stack.top();
                    automaton_stack.pop();
                    a2 = automaton_stack.top();
                    automaton_stack.pop();
                    automaton = a2 * a1;
                    automaton_stack.push(automaton);
                    break;
            }
        }
        else {
            tree_stack.emplace(node_index, true);
            const std::vector<int> &children = tree_node.get_children();
            for(const auto &child : children) {
                tree_stack.emplace(child, false);
            }
        }
    }
    return automaton_stack.top();
}

bool Regex::eval(const std::string &word) {
    return this->l_nfa.accept(word);
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
            int node = -1;
            switch(curr_prod) {
                case M_EXPR:
                case M_EXPR_PR:
                    if(value_stack.top() < 0) {
                        value_stack.pop();
                        break;
                    }
                    node = tree.emplace_node(SyntaxTreeNode::OR, 0);
                    tree.insert_child(node, value_stack.top());
                    value_stack.pop();
                    tree.insert_child(node, value_stack.top());
                    value_stack.pop();
                    break;
                case M_CONCAT:
                case M_CONCAT_PR:
                    if(value_stack.top() < 0) {
                        value_stack.pop();
                        break;
                    }
                    node = tree.emplace_node(SyntaxTreeNode::CONCAT, 0);
                    tree.insert_child(node, value_stack.top());
                    value_stack.pop();
                    tree.insert_child(node, value_stack.top());
                    value_stack.pop();
                    break;
                case M_STAR:
                    if(value_stack.top() < 0) {
                        value_stack.pop();
                        break;
                    }
                    node = tree.emplace_node(SyntaxTreeNode::STAR, 0);
                    tree.insert_child(node, value_stack.top());
                    value_stack.pop();
                    break;
                case M_END:
                    return tree;
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
    throw ExpressionNotRegex();
}

void Regex::set_expr(const std::string &new_expr) {
    this->expr = new_expr;
    this->tree = Parser::parse(new_expr);
    this->l_nfa = this->construct_nfa();
}