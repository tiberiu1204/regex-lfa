#include "lambda_nfa.h"

Edge::Edge(char trans_char, Node &dest) : trans_char(trans_char), dest(dest) {}

const Node &Edge::get_dest() const {
    return this->dest;
}

char Edge::get_trans_char() const {
    return this->trans_char;
}

Node::Node(int state) : state(state), is_terminal(false) {}

void Node::insert_edge(const Edge &edge) {
    this->edges.push_back(edge);
}

bool Node::check_is_terminal() const {
    return this->is_terminal;
}

int Node::get_state() const {
    return this->state;
}

void Node::set_terminal() {
    this->is_terminal = true;
}

const std::vector<Edge> &Node::get_edges() const {
    return this->edges;
}

void Automata::insert_node(int state) {
    this->nodes[state] = Node(state);
}

Automata::Automata() : init_state(0) {}

void Automata::insert_edge(int dest, int src, char tc) {
    this->nodes[src].insert_edge(Edge(tc, this->nodes[dest]));
}

void Automata::set_init_node(int state) {
    this->init_state = state;
}

void Automata::add_term_node(int state) {
    this->nodes[state].set_terminal();
    this->term_states.push_back(state);
}

bool Automata::accept(const std::string &word) {
    std::vector <std::tuple<int, int>> stack; //state, index
    std::unordered_map<int, std::set<int> > visited;
    stack.emplace_back(init_state, init_state);
    while (!stack.empty()) {
        int state = std::get<0>(stack.back());
        int index = std::get<1>(stack.back());
        stack.pop_back();

        if (index == word.length()) {
            if (nodes[state].check_is_terminal()) {
                return true;
            }
        }

        visited[state].insert(index);

        for (auto &edge: nodes[state].get_edges()) {
            int dest_state = edge.get_dest().get_state();
            if (edge.get_trans_char() == word[index]) {
                if (visited[dest_state].find(index + 1) == visited[dest_state].end()) {
                    stack.emplace_back(dest_state, index + 1);
                }
            } else if (edge.get_trans_char() == '-') {
                if (visited[dest_state].find(index) == visited[dest_state].end()) {
                    stack.emplace_back(dest_state, index);
                }
            }
        }
    }
    return false;
}