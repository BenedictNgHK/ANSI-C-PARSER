#include "Error.hpp"
void Error::addError(int line, const std::string &error)
{
    if (errors.count(line) == 0)
        errors[line] = error;
}

void Error::addGrammarError(int line, const std::string &error)
{
    grammarErrors.emplace_back(line, error);
}

void Error::printError(std::ostream &os)
{
    for (auto itr = errors.begin(); itr != errors.end(); ++itr)
    {
        os << "Line No. " << itr->first << "\t" << "Errors:\t";
        os << itr->second << "\n";
    }
    for (const auto &entry : grammarErrors)
    {
        os << "Line No. " << entry.first << "\t" << "Grammar Error:\t" << entry.second << "\n";
    }
}
