#include <filesystem>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <vector>
#include <map>
#include "Grammar_FNC.h"
#include "color.hpp"
const std::string file = "cyk.txt";
std::string read_file(const std::string& path) {
    std::ifstream in(path);
    std::stringstream buffer;
    buffer << in.rdbuf() << '\n';
    return buffer.str();
}
struct Token {
    std::string text;
    std::size_t line;
    Token(std::string text, std::size_t line) : text(std::move(text)), line(line) {}
};
std::vector<Token> tokenize(const std::string& data) {
    std::vector<Token> tokens;
    int begin = -1;
    std::size_t line = 1;
    for(int x = 0;x<data.size();x++) {
        switch(data[x]){
            case '\r':
            case '\n':
                line++;
                [[fallthrough]];
            case ' ':
                if(begin != -1) {
                    tokens.emplace_back(data.substr(begin, x - begin), line);
                    assert(!tokens.back().text.contains('\n'));
                    begin = -1;
                }
                if(data[x] == '\n') {
                    tokens.emplace_back("\n", line - 1);
                }
                break;
            default:
                if(begin == -1) {
                    begin = x;
                }
                break;
        }
    }
    return tokens;
}
class IDs {
    std::map<std::string, std::size_t> assignment;
    std::map<std::size_t, std::string> reverse;
    std::size_t nextid = 0;
public:
    std::size_t operator[](const std::string &str) {
        if(!assignment.contains(str)) {
            reverse[nextid] = str;
            assignment[str] = nextid++;
        }
        return assignment[str];
    }
    std::size_t operator[](const Token &tok) {
        return (*this)[tok.text];
    }
    std::string getById(std::size_t id) {
        if(!reverse.contains(id))
            throw std::invalid_argument("ID not found");
        return reverse[id];
    }
};
int main () {
    if(!std::filesystem::exists(file) || std::filesystem::is_directory(file)) {
        color("red", "File " + file + " not found!", true);
        return 1;
    }
    Grammar_FNC grammar;
    auto tokens = tokenize(read_file(file));
    IDs nt_id;
    auto tok = tokens.begin();
    grammar.set_start_symbol(nt_id[tok->text]);
    tok++;
    {
        auto expr_left = tokens.end();
        auto expr_mid = tokens.end(), expr_right = expr_mid;
        auto add_unit_prod = [&]() {
            if(expr_mid->text == "__SPACE__")
                expr_mid->text = " ";
            if(expr_mid->text.size() != 1)
                color("red", "Syntax error: multiple characters in unit production at line " + std::to_string(tok->line), true);
            assert(expr_right == tokens.end());
            grammar.addUnitProd({nt_id[expr_left->text], expr_mid->text.at(0)});
            expr_left = expr_mid = tokens.end();
            return 0;
        };
        auto add_prod = [&]() {
            assert(expr_mid != tokens.end());
            assert(expr_right != tokens.end());
            grammar.addProd({nt_id[*expr_left], nt_id[*expr_mid], nt_id[*expr_right]});
            expr_left = expr_mid = expr_right = tokens.end();
            return 0;
        };
        auto prod_expr_end = [&]() {
            if(expr_right != tokens.end()) {
                assert(expr_mid != tokens.end());
                return add_prod();
            } else if(expr_mid != tokens.end()) {
                return add_unit_prod();
            } else if(expr_left != tokens.end()) {
                color("red", "Syntax error: incomplete production at line " + std::to_string(tok->line), true);
                return 1;
            } else {
                color("red", "Syntax error: expected production at line " + std::to_string(tok->line), true);
                return 1;
            }
        };
        for(;tok!=tokens.end();tok++){
            assert(!tok->text.empty());
            assert((tok->text == "\n") || !(tok->text.contains('\n')));
            assert(!tok->text.contains('\r'));
            if(tok->text.starts_with("\n")) {
                assert(tok->text == "\n");
                if(expr_left != tokens.end()) {
                    if(prod_expr_end())
                        return 1;
                }
                continue;
            } else if(tok->text == "|") {
                auto saved_left = expr_left;
                if(prod_expr_end())
                    return 1;
                expr_left = saved_left;
            } else {
                if(expr_left == tokens.end())
                    expr_left = tok;
                else if(expr_mid == tokens.end())
                    expr_mid = tok;
                else if(expr_right == tokens.end())
                    expr_right = tok;
                else {
                    color("red", "Syntax error: too many tokens in production at line " + std::to_string(tok->line), true);
                    return 1;
                }
            }
        }
    }
    for(const auto &unused : grammar.getUnreachableNonterminals()) {
        color("yellow", "Warning: unreachable nonterminal " + nt_id.getById(unused) + " (" + std::to_string(unused) + ")", true);
    }
    for(const auto &empty : grammar.getEmptyNonterminals()) {
        color("yellow", "Warning: nonterminal " + nt_id.getById(empty) + " (" + std::to_string(empty) + ") leads to no productions", true);
    }
    while(true) {
        std::string input;
        std::getline(std::cin, input);
        if (input == "exit") break;
        if (grammar.cyk_check(input))
            color("green", "Accepted!", true);
        else
            color("red", "Rejected!", true);
    }
    return 0;
}
