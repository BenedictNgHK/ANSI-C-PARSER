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

    // Helper functions
    bool matchToken(TokenType expected);
    bool lookahead(TokenType expected);
    void expectToken(TokenType expected, const std::string &errorMsg = "");

    // Primary and expressions
    std::shared_ptr<Node> primaryExpression();
    std::shared_ptr<Node> constant();
    std::shared_ptr<Node> stringLiteral();
    std::shared_ptr<Node> genericSelection();
    std::shared_ptr<Node> genericAssocList();
    std::shared_ptr<Node> genericAssociation();

    // Expression parsing (top to bottom precedence)
    std::shared_ptr<Node> expression();
    std::shared_ptr<Node> assignmentExpression();
    std::shared_ptr<Node> conditionalExpression();
    std::shared_ptr<Node> logicalOrExpression();
    std::shared_ptr<Node> logicalAndExpression();
    std::shared_ptr<Node> inclusiveOrExpression();
    std::shared_ptr<Node> exclusiveOrExpression();
    std::shared_ptr<Node> andExpression();
    std::shared_ptr<Node> equalityExpression();
    std::shared_ptr<Node> relationalExpression();
    std::shared_ptr<Node> shiftExpression();
    std::shared_ptr<Node> additiveExpression();
    std::shared_ptr<Node> multiplicativeExpression();
    std::shared_ptr<Node> castExpression();
    std::shared_ptr<Node> unaryExpression();
    std::shared_ptr<Node> postfixExpression();
    std::shared_ptr<Node> unaryOperator();
    std::shared_ptr<Node> argumentExpressionList();
    std::shared_ptr<Node> assignmentOperator();
    std::shared_ptr<Node> constantExpression();

    // Declaration parsing
    std::shared_ptr<Node> declaration();
    std::shared_ptr<Node> declarationSpecifiers();
    std::shared_ptr<Node> initDeclaratorList();
    std::shared_ptr<Node> initDeclarator();
    std::shared_ptr<Node> storageClassSpecifier();
    std::shared_ptr<Node> typeSpecifier();
    std::shared_ptr<Node> typeQualifier();
    std::shared_ptr<Node> functionSpecifier();
    std::shared_ptr<Node> alignmentSpecifier();

    // Type and declarator parsing
    std::shared_ptr<Node> declarator();
    std::shared_ptr<Node> directDeclarator();
    std::shared_ptr<Node> pointer();
    std::shared_ptr<Node> typeQualifierList();
    std::shared_ptr<Node> parameterTypeList();
    std::shared_ptr<Node> parameterList();
    std::shared_ptr<Node> parameterDeclaration();
    std::shared_ptr<Node> identifierList();
    std::shared_ptr<Node> parseTypeName(); // Renamed to avoid conflict with member variable
    std::shared_ptr<Node> abstractDeclarator();
    std::shared_ptr<Node> directAbstractDeclarator();
    std::shared_ptr<Node> specifierQualifierList();

    // Struct and union
    std::shared_ptr<Node> structOrUnionSpecifier();
    std::shared_ptr<Node> structOrUnion();
    std::shared_ptr<Node> structDeclarationList();
    std::shared_ptr<Node> structDeclaration();
    std::shared_ptr<Node> structDeclaratorList();
    std::shared_ptr<Node> structDeclarator();

    // Enum
    std::shared_ptr<Node> enumSpecifier();
    std::shared_ptr<Node> enumeratorList();
    std::shared_ptr<Node> enumerator();
    std::shared_ptr<Node> enumerationConstant();

    // Initializer
    std::shared_ptr<Node> initializer();
    std::shared_ptr<Node> initializerList();
    std::shared_ptr<Node> designation();
    std::shared_ptr<Node> designatorList();
    std::shared_ptr<Node> designator();

    // Statement parsing
    std::shared_ptr<Node> statement();
    std::shared_ptr<Node> labeledStatement();
    std::shared_ptr<Node> compoundStatement();
    std::shared_ptr<Node> blockItemList();
    std::shared_ptr<Node> blockItem();
    std::shared_ptr<Node> expressionStatement();
    std::shared_ptr<Node> selectionStatement();
    std::shared_ptr<Node> iterationStatement();
    std::shared_ptr<Node> jumpStatement();

    // Static assert
    std::shared_ptr<Node> staticAssertDeclaration();

    // Top level parsing
    std::shared_ptr<Node> translationUnit();
    std::shared_ptr<Node> externalDeclaration();
    std::shared_ptr<Node> functionDefinition();
    std::shared_ptr<Node> declarationList();

    // Existing functions
    std::shared_ptr<Node> parsingFile(std::shared_ptr<Node> root);
    std::shared_ptr<Node> includeStmt();

    // Helper check functions
    inline bool typeSpecifier(const std::list<Token>::iterator &itr)
    {
        TokenType type = itr->type;
        switch (type)
        {
        case TokenType::INT_TYPE:
        case TokenType::CHAR_TYPE:
        case TokenType::DOULBLE_TYPE:
        case TokenType::FLOAT_TYPE:
        case TokenType::LONG_TYPE:
        case TokenType::SHORT_TYPE:
        case TokenType::VOID:
        case TokenType::UNSINGED:
        case TokenType::SIGNED:
        case TokenType::STRUCT:
        case TokenType::UNION:
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
        case TokenType::EXTERN:
        case TokenType::AUTO:
        case TokenType::STATIC:
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
        case TokenType::MUL_ASSIGN:
        case TokenType::DIV_ASSIGN:
        case TokenType::MOD_ASSIGN:
        case TokenType::ADD_ASSIGN:
        case TokenType::SUB_ASSIGN:
        case TokenType::LEFT_ASSIGN:
        case TokenType::RIGHT_ASSIGN:
        case TokenType::OR_ASSIGN:
        case TokenType::AND_ASSIGN:
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
