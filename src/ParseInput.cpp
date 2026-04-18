#include "ParseInput.hpp"
#include <sstream>
#include <iomanip>
#include <vector>
#include <unordered_set>

std::string floatToFormattedString(float num) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << num;
    std::string string = ss.str();
    string.erase(string.find_last_not_of('0') + 1, std::string::npos);
    if (string.back() == '.') string.pop_back();
    return string;
}

std::vector<int> getIntVector(const std::string& input) {
    std::vector<int> intVector;
    size_t start = 0;
    while (true) {
        auto comma = input.find(',', start);
        auto substr = input.substr(start, comma - start);
        if (!substr.empty()) {
            if (intTypesMap.contains(substr)) intVector.emplace_back(intTypesMap.at(substr));
            else intVector.emplace_back(std::strtol(substr.c_str(), nullptr, 10));
        }
        if (comma == std::string::npos) break;
        start = comma + 1;
    }
    return intVector;
}

std::vector<TypedFloat> getTypedFloatVector(const std::string& input) {
    std::vector<TypedFloat> typedFloatVector;
    size_t start = 0;
    while (true) {
        auto comma = input.find(',', start);
        auto substr = input.substr(start, comma - start);
        if (!substr.empty()) {
            auto num = std::strtof(substr.c_str(), nullptr);
            auto lastChar = substr.back();
            if (lastChar == 's' || lastChar == 'x') typedFloatVector.emplace_back(TypedFloat{num, 1});
            else if (lastChar == 'b' || lastChar == 'v' || lastChar == 'y') typedFloatVector.emplace_back(TypedFloat{num, 2});
            else typedFloatVector.emplace_back(TypedFloat{num, 0});
        }
        if (comma == std::string::npos) break;
        start = comma + 1;
    }
    return typedFloatVector;
}

std::vector<std::string> getStringVector(const std::string& input) {
    std::vector<std::string> stringVector;
    size_t start = 0;
    while (true) {
        auto comma = input.find(',', start);
        auto substr = input.substr(start, comma - start);
        if (!substr.empty()) {
            std::unordered_set<std::string> keywords {"layout", "trigger", "decoration", "deco", "passable", "detail", "hazard", "solid"};
            if (keywords.contains(substr)) stringVector.emplace_back(substr);
            else return {"any"};
        }
        if (comma == std::string::npos) break;
        start = comma + 1;
    }
    return stringVector;
}

std::string trimDuplicates(const std::string& input) {
    std::unordered_set<std::string> uniqueEntries;
    std::string output = "";
    size_t start = 0;
    while (true) {
        auto comma = input.find(',', start);
        auto substr = input.substr(start, comma - start);
        if (!substr.empty() && !uniqueEntries.contains(substr)) {
            uniqueEntries.insert(substr);
            output += (start == 0 ? "" : ",") + substr;
        }
        if (comma == std::string::npos) break;
        start = comma + 1;
    }
    return output;
}
