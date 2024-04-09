#include "lambda_nfa.h"
#include <queue>
#include <map>

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

[[maybe_unused]] void Automaton::print() const {
    std::cout<<"Initial state: "<<init_state<<"\n";
    for(const auto &state_node_p : nodes) {
        state_node_p.second.print();
    }
    std::cout<<"\n";
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

Automaton::CharSet Automaton::get_trans_char_set(const IntSet &state_set) const {
    CharSet trans_char_set;
    for(const auto &state : state_set) {
        const Node &node = this->nodes.at(state);
        for(const auto &edge : node.get_edges()) {
            if(edge.get_trans_char() == '-') throw NfaHasLambda();
            trans_char_set.insert(edge.get_trans_char());
        }
    }
    return trans_char_set;
}

bool Automaton::check_state_set_terminal(const IntSet &state_set) {
    for(const auto &state : state_set) {
        if(this->nodes[state].check_is_terminal()) {
            return true;
        }
    }
    return false;
}

Automaton::IntSet Automaton::get_state_set(const IntSet &state_set, char trans_char) const {
    IntSet new_state_set;

    for(const auto &state : state_set) {
        const Node &node = this->nodes.at(state);
        for(const auto &edge : node.get_edges()) {
            if(edge.get_trans_char() == trans_char) new_state_set.insert(edge.get_dest());
            else if(edge.get_trans_char() == '-') throw NfaHasLambda();
        }
    }

    return new_state_set;
}

Automaton Automaton::to_dfa() {
    Automaton result;
    int new_state_index = 0;
    result.init_state = 0;
    result.insert_node(0);

    std::queue<IntSet> queue;
    queue.push({this->init_state});
    std::map<IntSet, int> state_map;
    state_map[queue.front()] = 0;

    while(!queue.empty()) {
        IntSet state_set = queue.front();
        queue.pop();

        const CharSet trans_char_set = this->get_trans_char_set(state_set);

        for(const auto &trans_char : trans_char_set) {
            IntSet new_state_set = this->get_state_set(state_set, trans_char);
            if(!new_state_set.empty()) {
                if(state_map.find(new_state_set) == state_map.end()) {
                    queue.push(new_state_set);
                    result.insert_node(++new_state_index);
                    state_map[new_state_set] = new_state_index;
                }
                result.insert_edge(state_map.at(new_state_set), state_map.at(state_set), trans_char);
                if(this->check_state_set_terminal(new_state_set))
                    result.nodes[new_state_index].set_terminal(true);
            }
        }
    }

    return result;
}

std::istream &operator>>(std::istream &in, Automaton &automaton) {
    int num_states;
    in >> num_states;
    for(int i = 0; i < num_states; i++) {
        int state;
        in >> state;
        automaton.insert_node(state);
    }
    int num_trans;
    in >> num_trans;
    for(int i = 0; i < num_trans; i++) {
        int dest_state, src_state;
        char trans_char;
        in >> src_state >> dest_state >> trans_char;
        automaton.insert_edge(dest_state, src_state, trans_char);
    }
    int initial_state;
    in >> initial_state;
    automaton.init_state = initial_state;
    int num_term_nodes;
    in >> num_term_nodes;
    for(size_t i = 0; i < num_term_nodes; i++) {
        int state;
        in >> state;
        automaton.nodes[state].set_terminal(true);
    }

    return in;
}

