#include <algorithm>
#include <stdexcept>
#include "PushdownFSM.h"

void PushdownFSM::add_transition(const Transition &trans) {
    transitions[{trans.from_stack, trans.from_state, trans.from_symbol}].push_back(trans);
}

void PushdownFSM::add_final_state(std::size_t state) {
    final_states.insert(state);
}

void PushdownFSM::set_start_state(std::size_t start) {
    start_state = start;
}

void PushdownFSM::set_start_stack(std::size_t start) {
    start_stack = start;
}

void PushdownFSM::set_accept_mode(PushdownFSM::AcceptMode mode) {
    accept_mode = mode;
}

bool PushdownFSM::try_accept(const std::string &word) {
    struct Thread {
        std::size_t state;
        decltype(word.begin()) sym;
        std::vector<std::size_t> stack;
    };
    std::vector<Thread> threads = {{start_state, word.begin(), {start_stack}}};
    std::size_t tick = 0;
    while(!threads.empty()) {
        std::vector<Thread> next_tick;
        for(const auto &thread : threads) {
            const auto procTrans = [&](const Transition &trans) -> bool {
                auto newStack = thread.stack;
                newStack.pop_back();
                newStack.insert(newStack.begin(), trans.to_stack.rbegin(), trans.to_stack.rend());
                if(newStack.empty()) {
                    if((accept_mode == AcceptMode::EMPTY_STACK && thread.sym == word.end()) ||
                       (accept_mode == AcceptMode::BOTH && thread.sym == word.end() && final_states.contains(thread.state))) return true;
                }
                if(accept_mode == AcceptMode::FINAL_STATE && thread.sym == word.end() && final_states.contains(thread.state)) return true;
                next_tick.push_back(Thread{trans.to_state, trans.from_symbol == 0 ? thread.sym : thread.sym + 1, std::move(newStack)});
                return false;
            };
            if(thread.sym != word.end()) {
                for (const auto &trans: transitions[{thread.state, thread.stack.back(), *thread.sym}]) {
                    if(procTrans(trans)) return true;
                }
            }
            for(const auto &trans : transitions[{thread.state, thread.stack.back(), 0}]) {
                if(procTrans(trans)) return true;
            }
        }
        threads = std::move(next_tick);
        tick++;
        if(tick == max_ticks) {
            throw std::runtime_error("Tick limit reached. PDA may be in a loop."); // TODO is PDA halting decidable?
        }
    }
    return false;
}
