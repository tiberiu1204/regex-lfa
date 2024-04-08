#include "lambda_nfa.h"
#include "regex_engine.h"
#include <regex>

int main() {
    std::vector<std::string> regex_arr = {"ab(cd|ef)*", "abcdefg", "(abc)*", "abc*", "abcc*", "(ab|c)*", "abc(def(hij)*)*"};
    std::vector<std::string> match_arr = {"abcdefefcdefef", "abcdefg", "abcabcabc", "ab", "abccc", "abcccababc", "abcdefhijhijdefhijhij"};
    for(const auto &expr : regex_arr) {
        Regex reg(expr);
        std::regex reg_std(expr);
        for(const auto &match : match_arr) {
            if(reg.eval(match) != std::regex_match(match, reg_std)) {
                std::cout<<"Failed at regex '"<<expr<<"' with match word '"<<match<<"'\n";
                return -1;
            }
        }
    }
    std::cout<<"All tests passed\n";
}