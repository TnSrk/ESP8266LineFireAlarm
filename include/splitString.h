#ifndef SPLITSTRING
#define SPLITSTRING
#include <vector>
#include <sstream>
#include <algorithm>

std::vector<std::string> splitString(const std::string str, char delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = 0;
    while ((end = str.find(delimiter, start)) != std::string::npos) {
        //Serial.println("end=");Serial.println(end);
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(str.substr(start)); // Add the last token
    return tokens;
}

#endif //SPLITSTRING