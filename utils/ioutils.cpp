#include "ioutils.h"
#include <algorithm>

using namespace std;

namespace ioutils {
    vector<string> split(const string& s, const string& delimiter) {
        if (s.empty()) {
            return vector<string>{};
        }
        vector<string> tokens;
        auto sCopy = string(s);
        size_t pos, prevPos = 0;
        string token;
        while ((pos = sCopy.find(delimiter)) != string::npos) {
            token = sCopy.substr(0, pos);
            tokens.push_back(token);
            sCopy.erase(sCopy.begin() + pos, sCopy.begin() + pos + delimiter.length());
            prevPos = pos;
        }
        tokens.push_back(sCopy.substr(prevPos, sCopy.length() - prevPos));

        return tokens;
    }

    string trim(string s) {
        size_t i = 0;
        vector<char> whitespaces = {' ', '\t'};
        if (s.empty()) {
            return s;
        }

        while (std::find(whitespaces.begin(), whitespaces.end(), s[i]) != whitespaces.end()) {
            s.erase(s.begin() + i);
            i++;
        }
        i = s.length() - 1;
        while (std::find(whitespaces.begin(), whitespaces.end(), s[i]) != whitespaces.end()) {
            s.erase(s.begin() + i);
            i--;
        }

        return s;
    }

    string toLower(const string &s) {
        string out;
        for (int i = 0; i < s.length(); i++) {
            out += (char)(tolower(s[i]));
        }
        return out;
    }
}
