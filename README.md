# Script-AI

From real life scripts to android dreams, this AI can learn to write scripts for movies, tv shows, and other forms of acting.

## Table of Contents
- Building
- Concepts
- Running
- Examples
- Credits
- Contributing

## Building
simply `make`. This requires g++ (with c++17 support) and autotools.

## Concepts
- **Lexicon**: the complete set of meaningful units in a language. Essentially, what chunks of a sentence the AI recognizes.
- **Sentence**: the list of words, punctuation, or other parts of the lexicon that in sequence form a statement, question, exclamation, or command.
- **Translation**: the text humans read is inneficient for a robot to read. The text is broken up into parts of the lexicon to translate text to a format the AI recognizes.
- **Training Data**: real life data is used as an example of what the AI should imitate. A good AI will understand the deeper meaning and do more than imitate the data.
- **AI**: *artificial intelligence; machine learning;* the software that emulates intelligence by emulating nurons and neural connections in a neural network.

## Running
`./bin/script-ai` and type `help` for usage.

## Examples
- TV Scripts
- Movie Scripts
- Books

## Credits
- My partner Jean Young for tidying up a lot of data
- [Tiny DNN](https://github.com/tiny-dnn/tiny-dnn) - a machine learning library for C++
- [nlohmann/json](https://github.com/nlohmann/json) - a json library for C++
- [data world](https://data.world/data) - where I sourced the typo filled scripts from; a website for data sharing

## Contributing
Create a fork, an issue, or message me personally!