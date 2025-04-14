#include "calculator.hpp"
#include <fstream>
#include <cctype>
#include <algorithm>
#include <string>

// Reads each line of the input file into a vector of strings.
std::vector<std::string> loadFileLines(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream infile(filename);
    std::string line;
    while (std::getline(infile, line)) {
        lines.push_back(line);
    }
    return lines;
}

// Validates that the string is a valid double in the format (+|-)A(.B)
bool isValidDouble(const std::string& s) {
    if(s.empty()) return false;
    int i = 0;
    if(s[i]=='+' || s[i]=='-') i++;
    bool hasDigits = false;
    while(i < s.size() && std::isdigit(s[i])) { 
        hasDigits = true; 
        i++; 
    }
    if(i < s.size()){
        if(s[i]=='.'){
            i++;
            bool hasFrac = false;
            while(i < s.size() && std::isdigit(s[i])) { 
                hasFrac = true; 
                i++; 
            }
            return hasDigits && hasFrac && (i == s.size());
        }
        return false;
    }
    return hasDigits && (i == s.size());
}

// Helper: add two non-negative integer strings (assumes they contain only digits).
std::string addStringInts(const std::string &a, const std::string &b) {
    std::string result;
    int carry = 0;
    int i = a.size()-1, j = b.size()-1;
    while(i >= 0 || j >= 0 || carry) {
        int d1 = (i >= 0) ? a[i]-'0' : 0;
        int d2 = (j >= 0) ? b[j]-'0' : 0;
        int sum = d1 + d2 + carry;
        result.insert(result.begin(), (sum % 10) + '0');
        carry = sum / 10;
        i--; j--;
    }
    return result;
}

// Helper: remove leading zeros (keeping at least one digit).
std::string stripLeadingZeros(const std::string &s) {
    size_t pos = 0;
    while(pos < s.size()-1 && s[pos]=='0')
        pos++;
    return s.substr(pos);
}

// Helper: compare two non-negative integer strings.
// Returns 1 if a > b, -1 if a < b, or 0 if equal.
int compareStringInts(const std::string &a, const std::string &b) {
    std::string A = stripLeadingZeros(a);
    std::string B = stripLeadingZeros(b);
    if(A.size() != B.size())
        return (A.size() > B.size()) ? 1 : -1;
    if(A == B)
        return 0;
    return (A > B) ? 1 : -1;
}

// Helper: subtracts string b from string a (assumes a >= b), both non-negative.
std::string subtractStringInts(const std::string &a, const std::string &b) {
    std::string result;
    int borrow = 0;
    int i = a.size()-1, j = b.size()-1;
    while(i >= 0) {
        int d1 = a[i]-'0' - borrow;
        int d2 = (j >= 0) ? b[j]-'0' : 0;
        if(d1 < d2) { d1 += 10; borrow = 1; } else { borrow = 0; }
        int diff = d1 - d2;
        result.insert(result.begin(), diff + '0');
        i--; j--;
    }
    return stripLeadingZeros(result);
}

// Adds two double numbers (as strings) while keeping them in string format.
// It processes the integer and fractional parts separately.
std::string addStrings(const std::string &a, const std::string &b) {
    bool negA = false, negB = false;
    std::string sA = a, sB = b;
    if(!sA.empty() && sA[0]=='-') { negA = true; sA = sA.substr(1); }
    else if(!sA.empty() && sA[0]=='+') sA = sA.substr(1);
    if(!sB.empty() && sB[0]=='-') { negB = true; sB = sB.substr(1); }
    else if(!sB.empty() && sB[0]=='+') sB = sB.substr(1);
    
    auto splitParts = [](const std::string &s, std::string &intPart, std::string &fracPart) {
        size_t pos = s.find('.');
        if(pos == std::string::npos) { intPart = s; fracPart = "0"; }
        else { intPart = s.substr(0, pos); fracPart = s.substr(pos+1); }
    };
    std::string intA, fracA, intB, fracB;
    splitParts(sA, intA, fracA);
    splitParts(sB, intB, fracB);
    
    size_t fracLen = std::max(fracA.size(), fracB.size());
    while(fracA.size() < fracLen) fracA.push_back('0');
    while(fracB.size() < fracLen) fracB.push_back('0');
    
    std::string wholeA = stripLeadingZeros(intA + fracA);
    std::string wholeB = stripLeadingZeros(intB + fracB);
    
    std::string wholeResult;
    bool resultNeg = false;
    if(negA == negB) {
        wholeResult = addStringInts(wholeA, wholeB);
        resultNeg = negA;
    } else {
        int cmp = compareStringInts(wholeA, wholeB);
        if(cmp == 0)
            return "0";
        else if(cmp > 0) {
            wholeResult = subtractStringInts(wholeA, wholeB);
            resultNeg = negA;
        } else {
            wholeResult = subtractStringInts(wholeB, wholeA);
            resultNeg = negB;
        }
    }
    
    if(wholeResult.size() <= fracLen)
        wholeResult.insert(wholeResult.begin(), fracLen - wholeResult.size() + 1, '0');
    
    size_t intLen = wholeResult.size() - fracLen;
    std::string resInt = wholeResult.substr(0, intLen);
    std::string resFrac = wholeResult.substr(intLen);
    while(!resFrac.empty() && resFrac.back() == '0') resFrac.pop_back();
    
    std::string result = resFrac.empty() ? resInt : resInt + "." + resFrac;
    result = stripLeadingZeros(result);
    if(result.empty()) result = "0";
    if(resultNeg && result != "0") result = "-" + result;
    return result;
}

// Parses the given validated double string into a double value.
// Since the string is assumed valid, no exceptions are thrown.
double parse_number(const std::string &expression) {
    int i = 0;
    int n = expression.size();
    int sign = 1;
    if (expression[i] == '-') {
        sign = -1;
        i++;
    } else if (expression[i] == '+') {
        i++;
    }
    double value = 0.0;
    while (i < n && std::isdigit(expression[i])) {
        value = value * 10 + (expression[i] - '0');
        i++;
    }
    if (i < n && expression[i] == '.') {
        i++; // skip the decimal point
        double factor = 0.1;
        while (i < n && std::isdigit(expression[i])) {
            value += (expression[i] - '0') * factor;
            factor *= 0.1;
            i++;
        }
    }
    return sign * value;
}
