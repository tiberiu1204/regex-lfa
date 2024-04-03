//
// Created by Tiberiu Popescu on 07.03.2024.
//
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <set>

class Node;

/*
 * A class that represents an edge (or transition) in the Automata. It contains the transition char and the destination
 * node as a reference.
 *
 * If the transition char in the input will be '-', it will be considered a lambda transition.
 *
 * It has a print method used for debugging.
 */

class Edge {
private:
    const char trans_char;
    Node &dest;
public:
    explicit Edge(char trans_char, Node &dest) : trans_char(trans_char), dest(dest) {}
    Node &get_dest() {
        return dest;
    }
    [[nodiscard]] char get_trans_char() const {
        return trans_char;
    }
    void print() const;
};

/*
 * A class that represents a node in the automata. It is represented by a state and a list of edges (or transitions).
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
    explicit Node(int state = 0) : state(state), is_terminal(false) {}
    void insert_edge(const Edge &edge) {
        edges.push_back(edge);
    }
    [[nodiscard]] bool check_is_terminal() const {
        return is_terminal;
    }
    void set_terminal() {
        is_terminal = true;
    }
    void print() const {
        std::cout<<"State: "<<state<<"\nEdges: ";
        for(auto edge : edges) {
            edge.print();
        }
        std::cout<<"\n";
    }
    [[nodiscard]] int get_state() const {
        return state;
    };
    std::vector<Edge> &get_edges() {
        return edges;
    }
};

void Edge::print() const {
    std::cout<<"('"<<trans_char<<"', "<<dest.get_state()<<") ";
}

/*
 * A class that represents the Automata itself. It contains nodes, an initial state and a list of terminal states.
 *
 * It has a print method used for debugging.
 *
 * The class is also implemented to work with all types of automata.
 *
 */

class Automata {
private:
    std::unordered_map<int, Node> nodes;
    int init_state;
    std::vector<int> term_states;
public:
    Automata() : init_state(0) {}
    void insert_node(int state) {
        nodes[state] = Node(state);
    }
    void insert_edge(int dest, int src, char tc) {
        nodes[src].insert_edge(Edge(tc, nodes[dest]));
    }
    void set_init_node(int state) {
        init_state = state;
    }
    void add_term_node(int state) {
        nodes[state].set_terminal();
        term_states.push_back(state);
    }

    /*
     * The accept function. Iterative implementation was preferred over the recursive one because for very large words
     * the stack would run out of space and crash the program. In this way, the program is also more memory efficient.
     *
     */

    std::vector<std::tuple<int, int> > accept(const std::string &word) {
        std::vector<std::tuple<int, int, int, char> > stack; //state, prev_state, index, trans_char
        std::unordered_map<int, std::set<int> > visited;
        std::vector<std::tuple<int, int> > path; // state, index
        path.emplace_back(init_state, init_state);
        stack.emplace_back(init_state, init_state, 0, 0);
        while(!stack.empty()) {
            int state = std::get<0>(stack.back());
            int prev_state = std::get<1>(stack.back());
            int index = std::get<2>(stack.back());
            char trans_char = std::get<3>(stack.back());
            stack.pop_back();

            while(!path.empty() && std::get<1>(path.back()) > index) path.pop_back();
            while(!path.empty() && std::get<1>(path.back()) <= index) {
                if((std::get<0>(path.back()) != prev_state) ||
                   (std::get<1>(path.back()) == index && trans_char != '-')) {
                    path.pop_back();
                }
                else {
                    break;
                }
            }

            path.emplace_back(state, index);

            if(index == word.length()) {
                if(nodes[state].check_is_terminal()) {
                    return path;
                }
            }

            visited[state].insert(index);

            for(auto &edge : nodes[state].get_edges()) {
                int dest_state = edge.get_dest().get_state();
                if(edge.get_trans_char() == word[index]) {
                    if(visited[dest_state].find(index + 1) == visited[dest_state].end()) {
                        stack.emplace_back(dest_state, state, index + 1, edge.get_trans_char());
                    }
                }
                else if(edge.get_trans_char() == '-') {
                    if(visited[dest_state].find(index) == visited[dest_state].end()) {
                        stack.emplace_back(dest_state, state, index, edge.get_trans_char());
                    }
                }
            }
        }
        return {};
    }
    void print() const {
        std::cout<<"Initial state: "<<init_state<<"\nTerminal states: ";
        for(auto state : term_states) {
            std::cout<<state<<" ";
        }
        std::cout<<"\n";
        for(const auto &state_node_p : nodes) {
            state_node_p.second.print();
        }
    }
};

int main() {
    std::ifstream in("input.txt");
    std::ofstream out("output.txt");
    if(!in.is_open()) {
        std::cout<<"Input file could not be opened.\n";
        return -1;
    }
    Automata automata;
    int num_states;
    in >> num_states;
    for(int i = 0; i < num_states; i++) {
        int state;
        in >> state;
        automata.insert_node(state);
    }
    int num_trans;
    in >> num_trans;
    for(int i = 0; i < num_trans; i++) {
        int dest_state, src_state;
        char trans_char;
        in >> src_state >> dest_state >> trans_char;
        automata.insert_edge(dest_state, src_state, trans_char);
    }
    int initial_state;
    in >> initial_state;
    automata.set_init_node(initial_state);
    int num_term_nodes;
    in >> num_term_nodes;
    for(int i = 0; i < num_term_nodes; i++) {
        int state;
        in >> state;
        automata.add_term_node(state);
    }
    automata.print();
    int num_words;
    in >> num_words;
    for(int i = 0; i < num_words; i++) {
        std::string word;
        in >> word;
        std::vector<std::tuple<int, int> > path = automata.accept(word);
        if(!path.empty()) {
            out<<"DA: ";
            for(const auto &state : path) {
                out<<std::get<0>(state)<<" ";
            }
            out<<"\nTotal length of path in nodes: "<<path.size()<<"\n";
        }
        else {
            out<<"NU\n";
        }
    }
    in.close();
    out.close();
}