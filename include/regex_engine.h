#ifndef LAMBDANFA_REGEX_ENGINE_H
#define LAMBDANFA_REGEX_ENGINE_H

#include <string>
#include <vector>
#include <memory>

class SyntaxTree {
private:
    class Node;
public:
    enum NodeType {
        CONCAT,
        STAR,
        OR,
        LITERAL
    };

    void emplace_node(NodeType node);
private:
    std::vector<Node> nodes;

    class Node {
    public:
        explicit Node(NodeType type = CONCAT);
        void set_type(NodeType node_type);
        void insert_child(Node *node);
        [[nodiscard]] NodeType get_type() const;
    private:
        NodeType type;
        std::vector<Node *> children;
    };
};

class Regex {
public:
    Regex(std::string expr);
    bool eval(const std::string &word);
private:
    std::string expr;
    SyntaxTree tree;
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
