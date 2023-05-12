#include <vector>
#include <iostream>
#include <string>
std::vector<std::string> sp(std::vector<bool> bt) {
    std::vector<std::string> ret(bt.size());
    for(int x = 0;x<bt.size();x++)
        ret[x] = (bt[x] ? " " : "");
    return ret;
}
void rec_gen(std::vector<bool> bt, int pos) {
    if(pos == bt.size()) {
        auto s = sp(bt);
        std::vector<std::string> strs = {"2", "*", "2", "=", "...", "?"};
        std::string out;
        for(int x = 0;x<strs.size();x++) {
            out += s.at(x);
            out += strs.at(x);
        }
        out += s.back();
        std::cout << "TRUE:\n" + out + "\n";
    } else {
        bt[pos] = true;
        rec_gen(bt, pos + 1);
        bt[pos] = false;
        rec_gen(bt, pos + 1);
    }
}
int main () {
	rec_gen(std::vector<bool>(7, false), 0);
	return 0;
}

