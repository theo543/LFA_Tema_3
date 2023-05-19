#include <fstream>
#include <filesystem>
#include "color.hpp"
#include "PushdownFSM.h"
#include <map>
#include <algorithm>
#include <cassert>
const int SUCCESS_EXIT_CODE = 0;
const int FAILURE_EXIT_CODE = 1;
const std::string pda_file = "pda.txt";
int main() {
    PushdownFSM fsm;
    if(!std::filesystem::exists(pda_file) || std::filesystem::is_directory(pda_file)) {
        color("red", "PDA file " + pda_file + " not found!", true);
        return 1;
    }
    std::ifstream file(pda_file);
    std::string acceptingMode;
    file >> acceptingMode;
    std::size_t initialState;
    file >> initialState;
    fsm.set_start_state(initialState);
    std::string initialStack;
    file >> initialStack;
    assert(initialStack.size() == 1);
    fsm.set_start_stack(initialStack[0] - 'A');
    if(acceptingMode == "BOTH")
        fsm.set_accept_mode(PushdownFSM::AcceptMode::BOTH);
    else if(acceptingMode == "EMPTY_STACK")
        fsm.set_accept_mode(PushdownFSM::AcceptMode::EMPTY_STACK);
    else if(acceptingMode == "FINAL_STATE")
        fsm.set_accept_mode(PushdownFSM::AcceptMode::FINAL_STATE);
    else {
        color("red", "Invalid accepting mode " + acceptingMode + "!", true);
        return 1;
    }
    std::size_t t_nr;
    file >> t_nr;
    for(std::size_t x = 0;x<t_nr;x++) {
        std::string from_symbol_s, from_stack_s, to_stack_s;
        std::size_t from_state, to_state;
        file >> from_state >> to_state >> from_symbol_s >> from_stack_s >> to_stack_s;
        assert(from_symbol_s.size() == 1);
        assert(from_stack_s.size() == 1);
        if(from_symbol_s == "_") from_symbol_s = "";
        if(to_stack_s == "_") to_stack_s = "";
        auto from_stack = static_cast<std::size_t>(from_stack_s[0] - 'A');
        std::vector<std::size_t> to_stack;
        std::transform(to_stack_s.begin(), to_stack_s.end(), std::back_inserter(to_stack), [](char c) { return static_cast<std::size_t>(c - 'A'); });
        fsm.add_transition(PushdownFSM::Transition(from_state, from_stack, from_symbol_s[0], to_state, to_stack));
    }
    std::size_t final_state_nr;
    file >> final_state_nr;
    for(std::size_t x = 0;x<final_state_nr;x++) {
        std::size_t final_state;
        file >> final_state;
        fsm.add_final_state(final_state);
    }
    bool success = true;
    std::size_t accepting_tests;
    file >> accepting_tests;
    color("blue", "Running " + std::to_string(accepting_tests) + " accepting tests...", true);
    for(std::size_t x = 0;x<accepting_tests;x++) {
        std::string test;
        file >> test;
        bool accepts = fsm.try_accept(test);
        if(accepts) {
            color("green", "Accepted: " + test, true);
        } else {
            success = false;
            color("red", "Rejected: " + test, true);
        }
    }
    std::size_t rejecting_tests;
    file >> rejecting_tests;
    color("blue", "Running " + std::to_string(rejecting_tests) + " rejecting tests...", true);
    for(std::size_t x = 0;x<rejecting_tests;x++) {
        std::string test;
        file >> test;
        bool accepts = fsm.try_accept(test);
        if(accepts) {
            success = false;
            color("red", "Accepted: " + test, true);
        } else {
            color("green", "Rejected: " + test, true);
        }
    }
    return success ? SUCCESS_EXIT_CODE : FAILURE_EXIT_CODE;
}
