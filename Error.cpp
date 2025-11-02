#include "Error.hpp"
void Error::addError(int line, const std::string &error)
{
    if (errors.count(line) == 0)
        errors[line] = error;
}

void Error::printError(std::ostream &os)
{
    for (auto itr = errors.begin(); itr != errors.end(); ++itr)
    {
        os << "Line No. " << itr->first << "\t" << "Errors:\t";
        os << itr->second << "\n";
    }
}
