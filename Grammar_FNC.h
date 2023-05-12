#ifndef LFA_TEMA_3_GRAMMAR_FNC_H
#define LFA_TEMA_3_GRAMMAR_FNC_H

#include <vector>
#include <string>
class Grammar_FNC {
public:
    struct UnitProd{
        std::size_t in_symbol;
        char out_text;
        UnitProd(int in_symbol, char out_text) : in_symbol(in_symbol), out_text(out_text) {}
    };
    struct Prod {
        std::size_t in_symbol;
        std::size_t out_left_symbol;
        std::size_t out_right_symbol;
        Prod(int in_symbol, int out_left_symbol, int out_right_symbol) : in_symbol(in_symbol), out_left_symbol(out_left_symbol), out_right_symbol(out_right_symbol) {}
    };
    void addUnitProd(UnitProd);
    void addProd(Prod);
    bool cyk_check(const std::string &word, std::size_t start_nonterminal);
    [[nodiscard]] std::size_t getMaxNonterminals() const;
private:
    std::vector<UnitProd> unitProds{};
    std::vector<Prod> prods{};
};


#endif //LFA_TEMA_3_GRAMMAR_FNC_H
