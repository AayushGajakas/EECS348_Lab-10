#ifndef __CALCULATOR_HPP
#define __CALCULATOR_HPP

#include <string>
#include <vector>

bool isValidDouble(const std::string& s);
std::string addStrings(const std::string& a, const std::string& b);
std::vector<std::string> loadFileLines(const std::string& filename);
double parse_number(const std::string &expression);

#endif