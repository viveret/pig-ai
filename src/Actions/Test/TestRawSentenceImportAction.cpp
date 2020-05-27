#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../SentenceIterator.hpp"

using namespace ScriptAI;
using namespace Sql;

TestRawSentenceImportAction::TestRawSentenceImportAction(AIProgram *prog): MenuAction(prog) {

}

const char* TestRawSentenceImportAction::label() {
	return "test sentence import";
}

std::string TestRawSentenceImportAction::description() {
	return "Test the way sentences are imported for creating the lexicon";
}

void TestRawSentenceImportAction::run() {
	std::string line;
	while (true) {
		std::cout << "test sentence (q to quit): ";
		getline(std::cin, line);
		if (line != "q") {
			for (RawSentenceIterator it(line); it.good(); it.next()) {
				std::cout << it.word << std::endl;
			}
		} else {
			break;
		}
	}
	std::cout << "Success!" << std::endl;
}
