#include "lambda_nfa.h"

Edge::Edge(char trans_char, int dest) : trans_char(trans_char), dest(dest) {}

int Edge::get_dest() const {
    return this->dest;
}

char Edge::get_trans_char() const {
    return this->trans_char;
}

Edge::Edge(const Edge &other, const std::unordered_map<int, int> &new_keys) {
    this->trans_char = other.trans_char;
    this->dest = new_keys.at(other.dest);
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

void Node::set_terminal(bool is) {
    this->is_terminal = is;
}

const std::vector<Edge> &Node::get_edges() const {
    return this->edges;
}

void Automaton::insert_node(int state) {
    this->nodes[state] = Node(state);
}

Automaton::Automaton() : init_state(0) {}

void Automaton::insert_edge(int dest, int src, char tc) {
    this->nodes[src].insert_edge(Edge(tc, dest));
}

void Automaton::set_init_node(int state) {
    this->init_state = state;
}

void Automaton::add_term_node(int state) {
    this->nodes[state].set_terminal();
//    this->term_states.insert(state);
}

bool Automaton::accept(const std::string &word) {
    std::vector<std::tuple<int, int> > stack; //state, index
    std::unordered_map<int, std::unordered_set<int> > visited;
    stack.emplace_back(init_state, 0);
    while(!stack.empty()) {
        int state = std::get<0>(stack.back());
        int index = std::get<1>(stack.back());
        stack.pop_back();

        if(index == word.length()) {
            if(nodes[state].check_is_terminal()) {
                return true;
            }
        }

        visited[state].insert(index);

        for(auto &edge : nodes[state].get_edges()) {
            int dest_state = nodes[edge.get_dest()].get_state();
            if(edge.get_trans_char() == word[index]) {
                if(visited[dest_state].find(index + 1) == visited[dest_state].end()) {
                    stack.emplace_back(dest_state, index + 1);
                }
            }
            else if(edge.get_trans_char() == '-') {
                if(visited[dest_state].find(index) == visited[dest_state].end()) {
                    stack.emplace_back(dest_state, index);
                }
            }
        }
    }
    return false;
}

Node::Node(const Node &other, const std::unordered_map<int, int> &new_keys) {
    this->state = new_keys.at(other.state);
    this->is_terminal = other.is_terminal;
    for(const auto &edge : other.edges) {
        this->edges.emplace_back(edge, new_keys);
    }
}

Automaton Automaton::operator|(const Automaton &other) {
    Automaton result;
    int index = 0;
    std::unordered_map<int, int> new_keys;

    for(const auto &key_node : this->nodes) {
        new_keys[key_node.first] = index++;
    }
    for(const auto &key_node : this->nodes) {
        Node new_node = Node(key_node.second, new_keys);
        result.nodes[new_node.get_state()] = new_node;
    }
    Edge e1 = Edge('-', new_keys[this->init_state]);

    for(const auto &key_node : other.nodes) {
        new_keys[key_node.first] = index++;
    }
    for(const auto &key_node : other.nodes) {
        Node new_node = Node(key_node.second, new_keys);
        result.nodes[new_node.get_state()] = new_node;
    }
    Edge e2 = Edge('-', new_keys[other.init_state]);

    result.init_state = index;
    result.nodes[index] = Node(index);
    result.nodes[index].insert_edge(e1);
    result.nodes[index].insert_edge(e2);

    return result;
}

Automaton &Automaton::operator|=(const Automaton &other) {
    *this = *this | other;
    return *this;
}

void Node::print() const {
    std::cout<<"State: "<<state<<(this->is_terminal ? " terminal" : "")<<"\nEdges: ";
    for(auto edge : edges) {
        edge.print();
    }
    std::cout<<"\n";
}

void Edge::print() const {
    std::cout<<"('"<<this->trans_char<<"', "<<this->dest<<") ";
}

void Automaton::print() const {
    std::cout<<"Initial state: "<<init_state<<"\n";
    for(const auto &state_node_p : nodes) {
        state_node_p.second.print();
    }
}

Automaton Automaton::operator*(const Automaton &other) {
    Automaton result;
    int index = 0;
    std::unordered_map<int, int> new_keys;
    std::vector<int> term_states;

    for(const auto &key_node : this->nodes) {
        new_keys[key_node.first] = index++;
        if(key_node.second.check_is_terminal()) {
            term_states.push_back(index - 1);
        }
    }
    for(const auto &key_node : this->nodes) {
        Node new_node = Node(key_node.second, new_keys);
        new_node.set_terminal(false);
        result.nodes[new_node.get_state()] = new_node;
    }
    result.init_state = new_keys[this->init_state];

    for(const auto &key_node : other.nodes) {
        new_keys[key_node.first] = index++;
    }
    for(const auto &key_node : other.nodes) {
        Node new_node = Node(key_node.second, new_keys);
        result.nodes[new_node.get_state()] = new_node;
    }

    for(const auto &term : term_states) {
        result.nodes[term].insert_edge(Edge('-', new_keys[other.init_state]));
    }

    return result;
}

Automaton &Automaton::operator*=(const Automaton &other) {
    *this = *this * other;
    return *this;
}

Automaton::Automaton(char trans_char) {
    this->init_state = 0;
    this->insert_node(0);
    this->insert_node(1);
    this->nodes[1].set_terminal(true);
    this->insert_edge(1, 0, trans_char);
}

Automaton Automaton::operator*() {
    Automaton result;
    int index = 0;
    std::unordered_map<int, int> new_keys;
    std::vector<int> term_states;

    for(const auto &key_node : this->nodes) {
        new_keys[key_node.first] = index++;
        if(key_node.second.check_is_terminal()) {
            term_states.push_back(index - 1);
        }
    }
    for(const auto &key_node : this->nodes) {
        Node new_node = Node(key_node.second, new_keys);
        result.nodes[new_node.get_state()] = new_node;
    }

    result.init_state = index;
    result.nodes[index] = Node(index);
    result.nodes[index].set_terminal(true);
    result.nodes[index].insert_edge(Edge('-', new_keys[this->init_state]));

    for(const auto &term : term_states) {
        result.nodes[term].insert_edge(Edge('-', index));
    }

    return result;
}
