#include "MenuAction.hpp"
#include "StackOverflow.hpp"

#include <iostream>

using namespace PigAI;

MenuAction::MenuAction(AIProgram *prog): _prog(prog) {

}

MenuAction::~MenuAction() {

}

bool MenuAction::confirm() {
    std::cout << " (Y/n): ";
    std::string answer;
    std::cin >> answer;
    trim(answer);
    return answer != "n";
}