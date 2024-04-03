#include "lambda_nfa.h"
#include <fstream>

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
    int num_words;
    in >> num_words;
    for(int i = 0; i < num_words; i++) {
        std::string word;
        in >> word;
        if(automata.accept(word)) std::cout<<"DA\n";
        else std::cout<<"NU\n";
    }
    in.close();
    out.close();
}