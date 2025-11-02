#ifndef AST_HPP
#define AST_HPP
#include <memory>
#include "Scanner.hpp"
#include "Error.hpp"
#include <queue>
#include <array>
#include <set>

struct Node
{
    Token t;
    std::vector<std::shared_ptr<Node>> children;
    // Node(Token &&t){s}
    Node(Token t) : t(t) {};
    Node(TokenType type) : t(type, "") {};
    Node() = default;
};
class AST : public Scanner
{

protected:
    std::shared_ptr<Node> root;
    std::set<std::string> typeName;
    inline bool isTypeName(const std::string &name) { return typeName.count(name) > 0; }
    std::shared_ptr<Node> parsingFile(std::shared_ptr<Node> root);
    std::shared_ptr<Node> includeStmt();

    std::shared_ptr<Node> structUnionSpecifier(std::list<Token>::iterator begin);
    std::shared_ptr<Node> structDeclarationList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> structDeclaration(std::list<Token>::iterator begin);
    std::shared_ptr<Node> initDeclarator(std::list<Token>::iterator begin);

    std::shared_ptr<Node> structDeclaratorList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> structDeclarator(std::list<Token>::iterator begin);
    std::shared_ptr<Node> declarator(std::list<Token>::iterator begin);
    std::shared_ptr<Node> directDeclarator(std::list<Token>::iterator begin);
    std::shared_ptr<Node> identifierList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> parameterTypeList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> parameterList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> parameterDeclaration(std::list<Token>::iterator begin);
    std::shared_ptr<Node> declarationSpecifier(std::list<Token>::iterator begin);
    std::shared_ptr<Node> abstractDeclarator(std::list<Token>::iterator begin);
    std::shared_ptr<Node> directAbstractDeclarator(std::list<Token>::iterator begin);
    std::shared_ptr<Node> pointer(std::list<Token>::iterator begin);
    std::shared_ptr<Node> typeQualifierList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> specifierQualifierList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> enumSpecifier(std::list<Token>::iterator begin);
    std::shared_ptr<Node> enumerator(std::list<Token>::iterator begin);
    std::shared_ptr<Node> enumeratorList(std::list<Token>::iterator begin);
    std::shared_ptr<Node> initialzier(std::list<Token>::iterator begin);
    std::shared_ptr<Node> assignmentExp(std::list<Token>::iterator begin);
    inline bool typeSpecifier(const std::list<Token>::iterator &itr)
    {
        TokenType type = itr->type;
        switch (type)
        {
        case TokenType::INT_TYPE:
            return true;
        case TokenType::CHAR_TYPE:
            return true;
        case TokenType::DOULBLE_TYPE:
            return true;
        case TokenType::FLOAT_TYPE:
            return true;
        case TokenType::LONG_TYPE:
            return true;
        case TokenType::SHORT_TYPE:
            return true;
        case TokenType::VOID:
            return true;
        case TokenType::UNSINGED:
            return true;
        case TokenType::SIGNED:
            return true;

        case TokenType::STRUCT:
            return true;
        case TokenType::UNION:
            return true;
        case TokenType::ENUM:
            return true;

        default:
            return false;
        }
    }

    inline bool structUnion(const std::list<Token>::iterator &itr)
    {
        return itr->type == TokenType::STRUCT || itr->type == TokenType::UNION;
    }
    inline bool typeQualifier(const std::list<Token>::iterator &itr)
    {
        TokenType type = itr->type;
        switch (type)
        {
        case TokenType::CONST:
            return true;
        case TokenType::VOLATILE:
            return true;
        default:
            return false;
        }
    }
    inline bool storageClassSpecifier(const std::list<Token>::iterator &itr)
    {
        TokenType type = itr->type;
        switch (type)
        {
        case TokenType::TYPEDEF:
            return true;
        case TokenType::EXTERN:
            return true;
        case TokenType::AUTO:
            return true;
        case TokenType::STATIC:
            return true;
        case TokenType::REGISTER:
            return true;
        default:
            return false;
        }
    }
    inline bool assignOperator(const std::list<Token>::iterator &itr)
    {
        switch (itr->type)
        {
        case TokenType::ASSIGN:
            return true;

        case TokenType::MUL_ASSIGN:
            return true;
        case TokenType::DIV_ASSIGN:
            return true;
        case TokenType::MOD_ASSIGN:
            return true;
        case TokenType::ADD_ASSIGN:
            return true;
        case TokenType::SUB_ASSIGN:
            return true;
        case TokenType::LEFT_ASSIGN:
            return true;
        case TokenType::RIGHT_ASSIGN:
            return true;
        case TokenType::OR_ASSIGN:
            return true;
        case TokenType::AND_ASSIGN:
            return true;
        case TokenType::XOR_ASSIGN:
            return true;
        default:
            return false;
        }
    }
    std::set<std::string> definedStruct;
    std::set<std::string> definedUnion;

private:
    void printBranches(const std::vector<bool> &isLast, std::ostream &os);
    void traverseTree(std::shared_ptr<Node> node, std::vector<bool> &isLast, std::ostream &os);
    TokenToString t;

public:
    AST(const std::string &path, Error &e);
    void printAST(std::ostream &os);
};
#endif