#include "Scanner.hpp"
#include "Token.hpp"
#include "Error.hpp"
#include "AST.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Requires exact 1 arguemnt: path to file" << std::endl;
        return 0;
    }
    Error error;
    Scanner scanner(std::string(argv[1]), error);
    int i = 0;
    TokenToString t;
    auto itr = scanner.getNextToken();
    std::cout << t(itr->type) << std::endl;
    int thisline = scanner.getlineNo();

    for (auto itr = scanner.getNextToken(); itr->type != TokenType::END; itr = scanner.getNextToken())
    {
        if (thisline != scanner.getlineNo())
        {
            if (thisline != 1)
                std::cout << "line NO: " << thisline << std::endl;
            thisline = scanner.getlineNo();
        }
        std::cout << "Type: " << t(itr->type) << " Lexeme: " << itr->lexeme << std::endl;
    }
    AST tree(argv[1], error);
    error.printError(std::cout);
    scanner.printMacro(std::cout);
    tree.printAST(std::cout);

    return 0;
}