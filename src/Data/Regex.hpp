#ifndef PIGAI_REGEX_H
#define PIGAI_REGEX_H

#include <regex>
#include <string>
#include <vector>

namespace PigAI {
    const std::regex& rgx_import_sentence();
    const std::vector<std::string>& categories();
    std::vector<std::string> split(std::string str, std::regex rgx);
}


#endif