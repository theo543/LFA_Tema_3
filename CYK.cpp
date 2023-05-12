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
const std::string tests_file = "cyk_tests.txt";
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
            case '\n':
                [[fallthrough]];
            case '\r':
                [[fallthrough]];
            case ' ':
                if(begin != -1) {
                    tokens.emplace_back(data.substr(begin, x - begin), line);
                    assert(!tokens.back().text.contains('\n'));
                    begin = -1;
                }
                if(data[x] == '\n') {
                    tokens.emplace_back("\n", line - 1);
                    line++;
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
    color("blue", "Reading grammar from " + file + "...", true);
    auto tokens = tokenize(read_file(file));
    IDs nt_id;
    auto tok = tokens.begin();
    grammar.set_start_symbol(nt_id[tok->text]);
    tok++;
    {
        auto expr_left = tokens.end();
        bool expr_left_confirmed = false;
        auto expr_mid = tokens.end(), expr_right = expr_mid;
        auto end_expr = [&]() {
            expr_left = expr_mid = expr_right = tokens.end();
            expr_left_confirmed = false;
            return 0;
        };
        auto add_unit_prod = [&]() {
            if(expr_mid->text == "__SPACE__")
                expr_mid->text = " ";
            if(expr_mid->text.size() != 1)
                color("red", "Syntax error: multiple characters in unit production at line " + std::to_string(tok->line), true);
            assert(expr_right == tokens.end());
            grammar.addUnitProd({nt_id[expr_left->text], expr_mid->text.at(0)});
            end_expr();
            return 0;
        };
        auto add_prod = [&]() {
            assert(expr_mid != tokens.end());
            assert(expr_right != tokens.end());
            grammar.addProd({nt_id[*expr_left], nt_id[*expr_mid], nt_id[*expr_right]});
            end_expr();
            return 0;
        };
        auto prod_expr_end = [&]() {
            if(expr_right != tokens.end()) {
                assert(expr_mid != tokens.end());
                return add_prod();
            } else if(expr_mid != tokens.end()) {
                return add_unit_prod();
            } else if(expr_left != tokens.end()) {
                color("red", "Syntax error: incomplete production before | at line " + std::to_string(tok->line), true);
                return 1;
            } else {
                color("red", "Syntax error: expected production before | at line " + std::to_string(tok->line), true);
                return 1;
            }
        };
        for(;tok!=tokens.end();tok++){
            assert(!tok->text.empty());
            assert((tok->text == "\n") || !(tok->text.contains('\n')));
            assert(!tok->text.contains('\r'));
            if(tok->line == 1 && tok->text != "\n") {
                color("red", "Syntax error: line 1 should only contain the start symbol", true);
                return 1;
            }
            if(tok->text.starts_with("\n")) {
                assert(tok->text == "\n");
                if(expr_left != tokens.end()) {
                    if(prod_expr_end())
                        return 1;
                }
                continue;
            } else if(tok->text == "|") {
                auto saved_left = expr_left;
                auto saved_left_confirmed = expr_left_confirmed;
                if(prod_expr_end())
                    return 1;
                expr_left = saved_left;
                expr_left_confirmed = saved_left_confirmed;
            } else if(tok->text == "->") {
                if(expr_left == tokens.end()) {
                    color("red", "Syntax error: expected nonterminal before -> at line " + std::to_string(tok->line), true);
                    return 1;
                }
                if(expr_left_confirmed) {
                    color("red", "Syntax error: multiple -> at line " + std::to_string(tok->line), true);
                    return 1;
                }
                expr_left_confirmed = true;
            } else {
                if(expr_left == tokens.end())
                    expr_left = tok;
                else if(!expr_left_confirmed) {
                    color("red", "Syntax error: expected -> before nonterminal at line " + std::to_string(tok->line), true);
                    return 1;
                }
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
    if(!std::filesystem::exists(tests_file) || std::filesystem::is_directory(tests_file)) {
        color("yellow", "File " + tests_file + " not found! Skipping tests.", true);
    } else {
        color("blue", "Running tests from " + tests_file + "...", true);
        bool fail = false;
        std::string input, expected;
        std::ifstream tests(tests_file);
        int line = 0;
        while(std::getline(tests, expected)) {
            std::getline(tests, input);
            while(input.ends_with("\r") || input.ends_with("\n"))
                input.pop_back();
            line += 2;
            bool ex;
            if(expected.starts_with("TRUE:"))
                ex = true;
            else if(expected.starts_with("FALSE:"))
                ex = false;
            else {
                color("red", "Expected TRUE: or FALSE: at line " + std::to_string(line), true);
                return 1;
            }
            bool result = grammar.cyk_check(input);
            if(result != ex) {
                color("red",
                      std::string("Test got wrong result at line ") + std::to_string(line) + "!\n" +
                      "Input: " + input + "\n" +
                      "Expected output: " + std::string(ex ? "TRUE\n" : "FALSE\n") +
                      "Actual output: " + std::string(result ? "TRUE\n" : "FALSE\n"), false);
                fail = true;
            }
        }
        if(!fail)
            color("green", "All tests passed!", true);
    }
    color("black", "Manual input:", true);
    while(true) {
        std::string input;
        if(!std::getline(std::cin, input)) break;
        if(input.empty()) continue;
        if (input == "exit") break;
        if (grammar.cyk_check(input))
            color("green", "Accepted!", true);
        else
            color("red", "Rejected!", true);
    }
    return 0;
}
