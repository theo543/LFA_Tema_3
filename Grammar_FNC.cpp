#include <stdexcept>
#include <cassert>
#include <unordered_set>
#include "Grammar_FNC.h"
#include "Array3D.h"
void Grammar_FNC::addProd(Grammar_FNC::Prod p) {
    prods.push_back(p);
}

void Grammar_FNC::addUnitProd(Grammar_FNC::UnitProd up) {
    unitProds.push_back(up);
}

std::size_t Grammar_FNC::getMaxNonterminals() const {
    std::size_t nonterminals = 0;
    for(const auto &up : unitProds) {
        nonterminals = std::max(nonterminals, up.in_symbol);
    }
    for(const auto &p : prods) {
        std::size_t max_id = std::max(p.in_symbol, std::max(p.out_left_symbol, p.out_right_symbol));
        nonterminals = std::max(nonterminals, max_id);
    }
    return nonterminals;
}

bool Grammar_FNC::cyk_check(const std::string& word, std::size_t start_nonterminal) {
    if(word.empty()) throw std::invalid_argument("word is empty");
    std::size_t nonterminals = getMaxNonterminals();
    if(start_nonterminal >= nonterminals) throw std::out_of_range("start_nonterminal out of range");
    Array3D<bool> dp(word.size() + 1, word.size() + 1, nonterminals + 1, false);
    // dp.at(range_len, range_start, nonterminal)
    for(std::size_t x = 0;x<word.size();x++) {
        for(const auto &up : unitProds) {
            if(up.out_text == word[x]) {
                dp.at(1, x, up.in_symbol) = true;
            }
        }
    }
    for(std::size_t len = 2;len<=word.size();len++) {
        for(std::size_t start = 0;start < (word.size() - len + 1);start++) {
            for(std::size_t part_start = 1;part_start<len;part_start++) {
                auto first_start = start;
                auto second_start = start + part_start;
                auto first_len = part_start;
                auto second_len = len - part_start;
                assert(first_len < len);
                assert(second_len < len);
                for(const auto &p : prods) {
                    if(dp.at(first_len, first_start, p.out_left_symbol)
                    && dp.at(second_len, second_start, p.out_right_symbol)) {
                        dp.at(len, start, p.in_symbol) = true;
                    }
                }
            }
        }
    }
    return dp.at(word.size(), 0, start_nonterminal);
}
