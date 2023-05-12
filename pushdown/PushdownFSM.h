#ifndef LFA_TEMA_3_PUSHDOWNFSM_H
#define LFA_TEMA_3_PUSHDOWNFSM_H

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

class PushdownFSM {
public:
    enum class AcceptMode {
        EMPTY_STACK,
        FINAL_STATE,
        BOTH
    };
    struct Transition {
        std::size_t from_state, from_stack, to_state;
        char from_symbol;
        std::vector<std::size_t> to_stack;
    };
    void add_transition(const Transition &trans);
    void add_final_state(std::size_t state);
    bool try_accept(const std::string &word);
    void set_start_state(std::size_t start);
    void set_start_stack(std::size_t start);
    void set_accept_mode(AcceptMode mode);
private:
    struct From {
        std::size_t from_state, from_stack;
        char from_symbol;
        bool operator==(const From &f) const = default;
    };
    struct FromHasher {
        std::size_t operator()(const From &f) const {
            return (~std::hash<std::size_t>()(f.from_state) + 10247693) ^ (std::hash<std::size_t>()(f.from_stack) + 22466603) ^ std::hash<char>()(f.from_symbol);
        }
    };
    std::unordered_map<From, std::vector<Transition>, FromHasher> transitions;
    std::unordered_set<std::size_t> final_states;
    std::size_t start_state = 0, start_stack = 0;
    AcceptMode accept_mode = AcceptMode::BOTH;
    const static std::size_t max_ticks = 1000;
};

#endif //LFA_TEMA_3_PUSHDOWNFSM_H
