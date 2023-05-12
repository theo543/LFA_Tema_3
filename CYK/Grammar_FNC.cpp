#include <stdexcept>
#include <cassert>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include "Grammar_FNC.h"
#include "Array3D.h"
void Grammar_FNC::addProd(Grammar_FNC::Prod p) {
    prods.push_back(p);
}

void Grammar_FNC::addUnitProd(Grammar_FNC::UnitProd up) {
    unitProds.push_back(up);
}

std::size_t Grammar_FNC::getMaxNonterminals() const {
    std::size_t nonterminals = start_symbol;
    for(const auto &up : unitProds) {
        nonterminals = std::max(nonterminals, up.in_symbol);
    }
    for(const auto &p : prods) {
        std::size_t max_id = std::max(p.in_symbol, std::max(p.out_left_symbol, p.out_right_symbol));
        nonterminals = std::max(nonterminals, max_id);
    }
    return nonterminals;
}

bool Grammar_FNC::cyk_check(const std::string &word) {
    if(word.empty()) throw std::invalid_argument("word is empty");
    std::size_t nonterminals = getMaxNonterminals() + 1;
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
    return dp.at(word.size(), 0, start_symbol);
}

std::vector<std::size_t> Grammar_FNC::getUnreachableNonterminals() const {
    std::size_t nonterminals = getMaxNonterminals() + 1;
    std::vector<bool> reached(nonterminals, false);
    std::queue<std::size_t> bfs;
    std::vector<std::size_t> ret;
    const auto visit = [&](std::size_t nt) {
        if(!reached[nt]) {
            bfs.push(nt);
            reached[nt] = true;
        }
    };
    visit(start_symbol);
    std::unordered_map<std::size_t, std::vector<Prod>> nt_prods;
    for(const auto &p : prods) {
        nt_prods[p.in_symbol].push_back(p);
    }
    while(!bfs.empty()) {
        std::size_t current = bfs.front();
        bfs.pop();
        for(const auto &p : nt_prods[current]) {
            visit(p.out_left_symbol);
            visit(p.out_right_symbol);
        }
    }
    for(std::size_t x = 0;x<nonterminals;x++) {
        if(!reached[x])
            ret.push_back(x);
    }
    return ret;
}

std::vector<std::size_t> Grammar_FNC::getEmptyNonterminals() const {
    std::size_t nonterminals = getMaxNonterminals() + 1;
    std::vector<bool> reached(nonterminals, false);
    std::vector<std::size_t> ret;
    for(const auto &up : unitProds)
        reached[up.in_symbol] = true;
    for(const auto &p : prods)
        reached[p.in_symbol] = true;
    for(std::size_t x = 0;x<nonterminals;x++) {
        if(!reached[x])
            ret.push_back(x);
    }
    return ret;
}

void Grammar_FNC::set_start_symbol(std::size_t start) {
    start_symbol = start;
}
