#ifndef SCRIPTAI_REGEX_H
#define SCRIPTAI_REGEX_H

#include <regex>
#include <string>
#include <vector>

namespace ScriptAI {
    const std::regex& rgx_import_sentence();
    const std::vector<std::string> list_import_word_prefix();
    const std::vector<std::string> list_import_word_suffix();
    const std::vector<std::string> list_import_word_nofix();
    const std::regex& rgx_import_word_ignore();

    std::vector<std::string> split(std::string str, std::regex rgx);
}


#endif