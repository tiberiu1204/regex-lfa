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

class Parser {
public:
    static SyntaxTree parse(const std::string &word);
};

#endif //LAMBDANFA_REGEX_ENGINE_H
