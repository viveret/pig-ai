#ifndef STACKOVERFLOW_H
#define STACKOVERFLOW_H

#include <algorithm> 
#include <cctype>
#include <locale>


// https://stackoverflow.com/a/217605/11765486

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}







// trim from start (in place)
static inline void ltrim(std::string &s, int c) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [c](int ch) {
        return c != ch;
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s, int c) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [c](int ch) {
        return c != ch;
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s, int c) {
    ltrim(s, c);
    rtrim(s, c);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s, int c) {
    ltrim(s, c);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s, int c) {
    rtrim(s, c);
    return s;
}


#endif