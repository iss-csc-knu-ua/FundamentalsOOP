#pragma once

#include <string>
#include <iostream>
#include <sstream>

std::string representChar(char c) {
    switch (c) {
        case '\n': return "\\n";
        case '\t': return "\\t";
        case '\\': return "\\\\";
        case '\"': return "\\\"";
        default: return std::string(1, c); // Return the character itself
    }
}

std::string representChar(const std::string& str) {
    std::string result;
    for (char c : str) {
        result += representChar(c);
    }
    return result;
}

std::string showContext(const std::string& str1, size_t position) {
    std::ostringstream outStream;
    outStream<<'"';
    if (position > 5) {
        outStream << "..." << representChar(str1.substr(position - 5, 5));
    } else {
        outStream << representChar(str1.substr(0, position));
    }

    outStream << "[" << representChar(str1[position]) << "]";
    if (position < str1.length() - 1) {
        outStream << representChar(str1.substr(position + 1, 5));
        if (position + 5 < str1.length() - 1) {
            outStream << "...";
        }
    }
    outStream << '"';
    return outStream.str();
}

bool compareStrings(const std::string& str1, const std::string& str2, std::ostream& outStream = std::cout) {
    size_t minLength = std::min(str1.length(), str2.length());

    for (size_t i = 0; i < minLength; ++i) {
        if (str1[i] != str2[i]) {
            outStream << "Strings differ at position " << i << ":\n";
            outStream << "Character in str1: '" << representChar(str1[i]) << "'\n";
            outStream << "Character in str2: '" << representChar(str2[i]) << "'\n";
            outStream << "Context: ";
            
            // Print context before and after the differing character
            outStream<< showContext(str1, i) << " vs " << showContext(str2,i);
            outStream << std::endl;
            return false;
        }
    }

    // Check for length difference
    if (str1.length() != str2.length()) {
        outStream << "Strings differ in length:\n";
        outStream << "str1 length: " << str1.length() << ", str2 length: " << str2.length() << std::endl;
        return false;
    }

    // Strings are equal
    return true;
}