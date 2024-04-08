#ifndef LAMBDANFA_LAMBDA_NFA_H
#define LAMBDANFA_LAMBDA_NFA_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class Node;

/*
 * A class that represents an edge (or transition) in the Automaton. It contains the transition char and the destination
 * node as a reference.
 *
 * If the transition char in the input will be '-', it will be considered a lambda transition.
 *
 * It has a print method used for debugging.
 */

class Edge {
private:
    char trans_char;
    int dest;
public:
    explicit Edge(char trans_char, int dest);
    Edge(const Edge &other, const std::unordered_map<int, int> &new_keys);
    [[nodiscard]] int get_dest() const;
    [[nodiscard]] char get_trans_char() const;
};

/*
 * A class that represents a node in the Automaton. It is represented by a state and a list of edges (or transitions).
 *
 * It has a print method used for debugging.
 *
 */

class Node {
private:
    int state;
    bool is_terminal;
    std::vector<Edge> edges;
public:
    explicit Node(int state = 0);
    Node(const Node &other, const std::unordered_map<int, int> &new_keys);
    void insert_edge(const Edge &edge);
    [[nodiscard]] bool check_is_terminal() const;
    void set_terminal(bool is = true);
    [[nodiscard]] int get_state() const;
    [[nodiscard]] const std::vector<Edge> &get_edges() const;
};

/*
 * A class that represents the Automaton itself. It contains nodes, an initial state and a list of terminal states.
 *
 * It has a print method used for debugging.
 *
 * The class is also implemented to work with all types of Automaton.
 *
 */

class Automaton {
private:
    std::unordered_map<int, Node> nodes;
    int init_state;
//    std::unordered_set<int> term_states;
public:
    Automaton();
    void insert_node(int state);
    void insert_edge(int dest, int src, char tc);
    void set_init_node(int state);
    void add_term_node(int state);

    /*
     * The accept function. Iterative implementation was preferred over the recursive one because for very large words
     * the stack would run out of space and crash the program. In this way, the program is also more memory efficient.
     *
     */

    bool accept(const std::string &word);

    Automaton operator|(const Automaton &other);
    Automaton &operator|=(const Automaton &other);
    Automaton operator*(const Automaton &other);
    Automaton &operator*=(const Automaton &other);
    Automaton operator*();
};

#endif //LAMBDANFA_LAMBDA_NFA_H
