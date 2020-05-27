#include "Regex.hpp"
#include "Config.hpp"

#include <iostream>

namespace ScriptAI {
    struct RegexInstances {
        std::regex _rgx_import_sentence;
        std::vector<std::string> _list_import_word_prefix;
        std::vector<std::string> _list_import_word_suffix;
        std::vector<std::string> _list_import_word_nofix;
        std::regex _rgx_import_word_ignore;
    };

    RegexInstances *rgx_instances = nullptr;
    RegexInstances *ensure_instance();
}

using namespace ScriptAI;

const std::regex& ScriptAI::rgx_import_sentence() { return ensure_instance()->_rgx_import_sentence; }
const std::vector<std::string> ScriptAI::list_import_word_prefix() { return ensure_instance()->_list_import_word_prefix; }
const std::vector<std::string> ScriptAI::list_import_word_suffix() { return ensure_instance()->_list_import_word_suffix; }
const std::vector<std::string> ScriptAI::list_import_word_nofix() { return ensure_instance()->_list_import_word_nofix; }
const std::regex& ScriptAI::rgx_import_word_ignore() { return ensure_instance()->_rgx_import_word_ignore; }

std::regex list_rgx("\\|", std::regex::ECMAScript | std::regex::icase);

std::vector<std::string> ScriptAI::split(std::string str, std::regex rgx) {
    std::vector<std::string> ret;
    std::regex_token_iterator<std::string::iterator> end;

    for (std::regex_token_iterator<std::string::iterator> it(str.begin(), str.end(), rgx, -1); it != end; it++) {
        ret.push_back(it->str());
    }

    return ret; 
}

RegexInstances* ScriptAI::ensure_instance() {
    if (rgx_instances == nullptr) {
        rgx_instances = new RegexInstances {
            std::regex(Config::Get_State("import_sentence_regex")->asCString(), std::regex::ECMAScript | std::regex::icase),
            split(Config::Get_State("import_word_prefix_list")->asCString(), list_rgx),
            split(Config::Get_State("import_word_suffix_list")->asCString(), list_rgx),
            split(Config::Get_State("import_word_nofix_list")->asCString(), list_rgx),
            std::regex(Config::Get_State("import_word_ignore_regex")->asCString(), std::regex::ECMAScript | std::regex::icase),
        };
    }
    return rgx_instances;
}