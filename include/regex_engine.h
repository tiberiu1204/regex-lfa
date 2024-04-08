#ifndef LAMBDANFA_REGEX_ENGINE_H
#define LAMBDANFA_REGEX_ENGINE_H

#include <string>
#include <vector>
#include <memory>
#include "lambda_nfa.h"

class ExpressionNotRegex : std::exception {};

class SyntaxTreeNode {
public:
    enum NodeType {
        CONCAT,
        STAR,
        OR,
        LITERAL
    };
    explicit SyntaxTreeNode(NodeType type, char ch);
    void set_type(NodeType node_type);
    void insert_child(int node_index);
    [[nodiscard]] NodeType get_type() const;
    [[nodiscard]] const std::vector<int> &get_children() const;
    [[nodiscard]] char get_value() const;
private:
    NodeType type;
    char value;
    std::vector<int> children;
};

class SyntaxTree {
public:
    int emplace_node(SyntaxTreeNode::NodeType type, char value);
    void insert_child(int father_index, int child_index);
    [[nodiscard]] const std::vector<SyntaxTreeNode> &get_nodes() const;
    [[nodiscard]] int root_index() const;
private:
    std::vector<SyntaxTreeNode> nodes;
};

class Regex {
public:
    explicit Regex(std::string expr);
    bool eval(const std::string &word);
    void set_expr(const std::string &new_expr);
private:
    Automaton l_nfa;
    std::string expr;
    SyntaxTree tree;

    Automaton construct_nfa();
};

/*
 * GRAMMAR:
 * expr :- concat expr' $ ;
 * expr' :- '|' concat expr' | epsilon ;
 * concat :- star concat' ;
 * concat' :- star concat' | epsilon ;
 * star :- primary star' ;
 * star' :- '*' | epsilon
 * primary :- literal | '(' expr ')' ;
 *
 * FIRST(expr) = { literal, ( }
 * FIRST(expr') = { |, epsilon }
 * FIRST(concat) = { literal, ( }
 * FIRST(concat') = { literal, (, epsilon }
 * FIRST(star) = { literal, ( }
 * FIRST(star') = { *, epsilon }
 * FIRST(primary) = { literal, ( }
 *
 * FORWARD(expr) = { ), $ }
 * FORWARD(expr') = { ), $ }
 * FORWARD(concat) = { |, ), $ }
 * FORWARD(concat') = { |, ), $ }
 * FORWARD(star) = { literal, (, |, ), $ }
 * FORWARD(star') = { literal, (, |, ), $ }
 * FORWARD(primary) = { *, literal, (, |, ), $ }
 *
 */

/*
 *  Using LL(1) algorithm to parse the regex expression according to the grammar described above and also building the
 *  AST (class SyntaxTree) in the process.
 */

class Parser {
public:
    static SyntaxTree parse(const std::string &word);
private:
    enum Symbol {
        P_EXPR = 0,
        P_EXPR_PR = 1,
        P_CONCAT = 2,
        P_CONCAT_PR = 3,
        P_STAR = 4,
        P_STAR_PR = 5,
        P_PRIMARY = 6,
        P_EPSILON = 7,
        M_EXPR,
        M_EXPR_PR,
        M_CONCAT,
        M_CONCAT_PR,
        M_STAR,
        M_STAR_PR,
        M_END,
        P_STAR_T,    // 0
        P_OR_T,      // 1
        P_LPAREN_T,  // 2
        P_RPAREN_T,  // 3
        P_LITERAL_T, // 4
        EOF_T        // 5
    };

    static constexpr size_t prod_count = 7;
    static constexpr size_t terminal_count = 6;
    static std::vector<Symbol> prod_table[prod_count][terminal_count];

    static Symbol char_to_symbol(char ch);
};

#endif //LAMBDANFA_REGEX_ENGINE_H
