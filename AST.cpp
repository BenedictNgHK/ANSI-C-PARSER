#include "AST.hpp"

AST::AST(const std::string &path, Error &e) : Scanner(path, e), root(std::make_shared<Node>(Node(TokenType::TRANSLATION_UNIT)))
{
    root->t.lexeme = path;
    root = parsingFile(root);
}
std::shared_ptr<Node> AST::parsingFile(std::shared_ptr<Node> root)
{

    TokenToString t;

    for (auto itr = getNextToken(); itr->type != TokenType::END; itr = getNextToken())
    {

        if (itr->type == TokenType::INCLUDE)
        {
            root->children.push_back(includeStmt());
            // continue;
        }

        // if (structUnion(itr))
        // {
        //     root->children.push_back(structUnionSpecifier(std::move(itr)));
        // }
        else if (storageClassSpecifier(itr) || typeSpecifier(itr) || typeQualifier(itr))
        {

            // root->children.push_back(declarationSpecifier(itr));
            root->children.push_back(parameterDeclaration(itr));
        }
        // else if (itr->type == TokenType::ENUM)
        // {
        //     root->children.push_back(enumSpecifier(itr));
        // }
        // if (itr->type == TokenType::MUL)
        // {
        //     root->children.push_back(pointer(itr));
        // }
    }

    return root;
}
std::shared_ptr<Node> AST::includeStmt()
{
    auto itr = getNextToken();
    Node stmt(TokenType::INCLUDE_STMT);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));

    auto tempType = itr->type;
    if (itr->type == TokenType::DOUBLE_QUOTE || itr->type == TokenType::LT)
        ret->children.push_back(std::make_shared<Node>(std::move(*itr)));

    else
    {
        loggedError.addError(lineNo, INCLUD_ERROR);
        return ret;
    }
    itr = getNextToken();

    if (itr->type == TokenType::INCLUDE_PATH)
        ret->children.push_back(std::make_shared<Node>(std::move(*itr)));
    else
    {
        loggedError.addError(lineNo, INCLUD_ERROR);
        return ret;
    }
    itr = getNextToken();

    if ((itr->type == TokenType::DOUBLE_QUOTE && tempType == TokenType::DOUBLE_QUOTE) || (itr->type == TokenType::GT && tempType == TokenType::LT))
        ret->children.push_back(std::make_shared<Node>(std::move(*itr)));
    else
    {
        loggedError.addError(lineNo, INCLUD_ERROR);
        return ret;
    }
    return ret;
}

std::shared_ptr<Node> AST::structUnionSpecifier(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::STRUCT_UNION_SPECIFIER);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    auto itr = getNextToken();
    if (itr->type == TokenType::ID)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*itr)));
        itr = peekNextToken();
        if (itr->type == TokenType::L_CUR)
        {
            itr = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*itr)));
            itr = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*structDeclarationList(itr))));
            itr = getNextToken();
            if (itr->type != TokenType::R_CUR)
            {
                loggedError.addError(lineNo, "Missing right curly bracket");
                ungetToken();
            }
            else
                ret->children.push_back(std::make_shared<Node>(std::move(*(itr))));
        }
    }
    else if (itr->type == TokenType::L_CUR)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*itr)));

        itr = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*structDeclarationList(itr))));
        itr = getNextToken();
        if (itr->type != TokenType::R_CUR)
        {
            loggedError.addError(lineNo, "Missing right curly bracket");
            ungetToken();
        }
        else
            ret->children.push_back(std::make_shared<Node>(std::move(*(itr))));
    }
    else
        loggedError.addError(lineNo, STRUCT_UNION_ERROR);
    return ret;
}
std::shared_ptr<Node> AST::structDeclarationList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::STRUCT_DECLARATION_LIST);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    while (1)
    {
        if (peekNextToken()->type == TokenType::R_CUR)
            break;
        begin = getNextToken();
        ret->children.push_back(structDeclaration(begin));
    }
    return ret;
}
std::shared_ptr<Node> AST::structDeclaration(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::STRUCT_DECLARATION);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(specifierQualifierList(begin));
    begin = getNextToken();
    ret->children.push_back(structDeclaratorList(begin));
    begin = getNextToken();
    if (begin->type == TokenType::SEMI_COLON)
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    else
        loggedError.addError(lineNo, "STRUCT DECLARATION ERROR");
    return ret;
}
std::shared_ptr<Node> AST::structDeclaratorList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::STRUCT_DECLARATION_LIST);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));

    ret->children.push_back(structDeclarator(begin));

    while (peekNextToken()->type == TokenType::COMMA)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(structDeclarator(begin));
    }
    return ret;
}
std::shared_ptr<Node> AST::structDeclarator(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::STRUCT_DECLARATOR);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    if (begin->type != TokenType::COLON)
    {
        ret->children.push_back(declarator(begin));
        begin = peekNextToken();
        if (begin->type == TokenType::COLON)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            if (peekNextToken()->type == TokenType::CONSTANT)
            {
                begin = getNextToken();
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
        }
    }
    else
    {
        if (begin->type == TokenType::COLON)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = peekNextToken();
            if (begin->type == TokenType::CONSTANT)
            {
                begin = getNextToken();
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
        }
    }
    return ret;
}
std::shared_ptr<Node> AST::declarator(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::DECLARATOR);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    if (begin->type == TokenType::MUL)
    {
        ret->children.push_back(pointer(begin));
        begin = getNextToken();
    }
    ret->children.push_back(directDeclarator(begin));
    return ret;
}
std::shared_ptr<Node> AST::directDeclarator(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::DIRECT_DECLARATOR);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    if (begin->type == TokenType::ID)
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    else if (begin->type == TokenType::L_BR)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(declarator(begin));
        begin = getNextToken();
        if (begin->type == TokenType::R_BR)
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    }
    while (peekNextToken()->type == TokenType::L_SQR || peekNextToken()->type == TokenType::L_BR)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        auto next = getNextToken();
        if (begin->type == TokenType::L_SQR)
        {
            if (next->type == TokenType::R_SQR)
                ret->children.push_back(std::make_shared<Node>(std::move(*next)));
            else
            {
                if (next->type == TokenType::CONSTANT)
                    ret->children.push_back(std::make_shared<Node>(std::move(*next)));
                next = getNextToken();
                if (next->type == TokenType::R_SQR)
                    ret->children.push_back(std::make_shared<Node>(std::move(*next)));
            }
        }
        else if (begin->type == TokenType::L_BR)
        {
            if (next->type == TokenType::R_BR)
                ret->children.push_back(std::make_shared<Node>(std::move(*next)));

            else if (next->type == TokenType::ID)
                ret->children.push_back(identifierList(next));
            else
                ret->children.push_back(parameterTypeList(next));
            next = getNextToken();
            if (next->type == TokenType::R_BR)
                ret->children.push_back(std::make_shared<Node>(std::move(*next)));
        }
    }

    return ret;
}
std::shared_ptr<Node> AST::identifierList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::IDENTIFIER_LIST);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));

    if (begin->type == TokenType::ID)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        while (peekNextToken()->type == TokenType::COMMA)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = peekNextToken();
            if (begin->type == TokenType::ID)
            {
                begin = getNextToken();
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
        }
    }
    return ret;
}
std::shared_ptr<Node> AST::parameterTypeList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::PARAMETER_TYPE_LIST);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(parameterList(begin));
    if (peekNextToken()->type == TokenType::COMMA)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        if (peekNextToken()->type == TokenType::ELLIPSIS)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
    }
    return ret;
}
std::shared_ptr<Node> AST::parameterList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::PARAMETER_LIST);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(parameterDeclaration(begin));
    while (peekNextToken()->type == TokenType::COMMA)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(parameterDeclaration(begin));
    }
    return ret;
}
// Debugging
std::shared_ptr<Node> AST::parameterDeclaration(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::PARAMETER_DECLARATION);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(declarationSpecifier(begin));
    begin = peekNextToken();
    if (begin->type == TokenType::MUL || begin->type == TokenType::ID || begin->type == TokenType::L_BR || begin->type == TokenType::L_SQR)
    {
        begin = getNextToken();
        if (begin->type == TokenType::ID)
            ret->children.push_back(declarator(begin));
        else if (begin->type == TokenType::L_SQR)
            ret->children.push_back(abstractDeclarator(begin));

        else
        {
            auto itr = begin;
            if (begin->type == TokenType::MUL)
            {
                auto type = itr->type;
                while (type == TokenType::MUL || type == TokenType::CONST || type == TokenType::VOLATILE)
                {
                    appendList(symbolTable);
                    itr = std::next(itr);
                    type = itr->type;
                }
                if ((type != TokenType::L_BR && type != TokenType::ID && type != TokenType::L_SQR) || type == TokenType::L_SQR)
                {
                    ret->children.push_back(abstractDeclarator(begin));
                    return ret;
                }
                if (type == TokenType::ID)
                {
                    ret->children.push_back(declarator(begin));
                    return ret;
                }
            }

            int bracketCount = 1;
            bool isDeclarator = false;
            for (; bracketCount != 0 && itr != symbolTable.end(); itr = std::next(itr))
            {
                appendList(symbolTable);
                if (itr->type == TokenType::L_BR)
                {
                    bracketCount++;
                    auto next = std::next(itr);

                    if (next->type == TokenType::ID && std::next(next)->type == TokenType::R_BR)
                    {

                        isDeclarator = true;
                        break;
                    }
                    if (std::next(itr)->type == TokenType::MUL && std::next(itr) != symbolTable.end())
                    {
                        auto type = std::next(itr)->type;
                        itr = std::next(itr);
                        while (type == TokenType::MUL || type == TokenType::CONST || type == TokenType::VOLATILE)
                        {
                            appendList(symbolTable);
                            itr = std::next(itr);
                            type = itr->type;
                        }
                        itr = std::prev(itr);
                    }
                }
                else if (itr->type == TokenType::R_BR)
                    bracketCount--;
            }
            if (isDeclarator)
                ret->children.push_back(declarator(begin));
            else
                ret->children.push_back(abstractDeclarator(begin));
        }
    }
    return ret;
}
std::shared_ptr<Node> AST::declarationSpecifier(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::DECLARATION_SPECIFIERS);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    auto start = begin;
    if (storageClassSpecifier(begin) || typeSpecifier(begin) || typeQualifier(begin) || (!begin->lexeme.empty() && isTypeName(begin->lexeme)))
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = peekNextToken();
        if ((storageClassSpecifier(begin) || typeSpecifier(begin) || typeQualifier(begin)) || (!begin->lexeme.empty() && isTypeName(begin->lexeme)))
        {
            begin = getNextToken();
            if (structUnion(begin))

                ret->children.push_back(structUnionSpecifier(begin));
            else if (begin->type == TokenType::ENUM)
                ret->children.push_back(enumSpecifier(begin));
            else
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
        while (1)
        {
            begin = peekNextToken();
            if (storageClassSpecifier(begin) || typeSpecifier(begin) || typeQualifier(begin) || (!begin->lexeme.empty() && isTypeName(begin->lexeme)))
            {
                begin = getNextToken();
                if (structUnion(begin))

                    ret->children.push_back(structUnionSpecifier(begin));
                else if (begin->type == TokenType::ENUM)
                    ret->children.push_back(enumSpecifier(begin));
                else
                    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
            else
                break;
        }
        if (start->type == TokenType::TYPEDEF)
        {
            auto itr = currentToken;
            while (itr->type != TokenType::ID)
            {

                appendList(symbolTable);
                itr = std::next(itr);
            }
            typeName.insert(itr->lexeme);
        }
    }
    return ret;
}
std::shared_ptr<Node> AST::abstractDeclarator(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::ABSTRACT_DECLARATOR);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));

    if (begin->type == TokenType::MUL)
    {
        ret->children.push_back(pointer(begin));
        auto peeked = peekNextToken();
        if (peeked->type == TokenType::L_BR || peeked->type == TokenType::L_SQR)
        {
            begin = getNextToken();
            ret->children.push_back(directAbstractDeclarator(begin));
        }
    }
    else if (begin->type == TokenType::L_SQR || begin->type == TokenType::L_BR)
        ret->children.push_back(directAbstractDeclarator(begin));
    return ret;
}
std::shared_ptr<Node> AST::initDeclarator(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::INIT_DECLARATOR);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(declarator(begin));
    if (peekNextToken()->type == TokenType::ASSIGN)
    {
        begin = getNextToken();
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(initialzier(begin));
    }
    return ret;
}
std::shared_ptr<Node> AST::initialzier(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::INITIALIZER);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    if (begin->type == TokenType::L_CUR)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
    }
}
// std::shared_ptr<Node> AST::assignmentExp(std::list<Token>::iterator begin)
// {
// }

std::shared_ptr<Node> AST::directAbstractDeclarator(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::DIRECT_ABSTRACT_DECLARATOR);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    if (begin->type == TokenType::L_BR)
    {

        begin = getNextToken();
        if (begin->type == TokenType::R_BR)
        {
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }

        else if (storageClassSpecifier(begin) || typeSpecifier(begin))
        {
            ret->children.push_back(parameterTypeList(begin));
            begin = getNextToken();

            if (begin->type == TokenType::R_BR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
        else
        {
            ret->children.push_back(abstractDeclarator(begin));
            begin = getNextToken();

            if (begin->type == TokenType::R_BR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
    }
    else if (begin->type == TokenType::L_SQR)
    {
        begin = getNextToken();
        if (begin->type == TokenType::R_SQR)
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        else
        {
            for (int i = 0; i < 2; i++)
            {
                if (i == 0)
                {
                    if (begin->type == TokenType::R_SQR)
                    {
                        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                        break;
                    }
                    else if (begin->type == TokenType::CONSTANT)
                    {
                        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                        begin = getNextToken();
                    }
                }
                if (i == 1 && begin->type == TokenType::R_SQR)
                    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
        }
    }
    while (1)
    {
        auto peeked = peekNextToken();
        if (peeked->type == TokenType::L_BR)
        {

            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));

            begin = getNextToken();
            if (begin->type == TokenType::R_BR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            else if (storageClassSpecifier(begin) || typeSpecifier(begin))
            {
                ret->children.push_back(parameterTypeList(begin));
                begin = getNextToken();

                if (begin->type == TokenType::R_BR)
                    ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
        }
        else if (peeked->type == TokenType::L_SQR)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));

            begin = getNextToken();

            if (begin->type == TokenType::R_SQR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            else
            {
                for (int i = 0; i < 2; i++)
                {
                    if (i == 0)
                    {
                        if (begin->type == TokenType::R_SQR)
                        {
                            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                            break;
                        }
                        else if (begin->type == TokenType::CONSTANT)
                        {
                            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                            begin = getNextToken();
                        }
                    }
                    if (i == 1 && begin->type == TokenType::R_SQR)
                        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
                }
            }
        }
        else
            break;
    }

    return ret;
}
std::shared_ptr<Node> AST::pointer(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::POINTER);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    if (begin->type == TokenType::MUL)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = peekNextToken();
        if (begin->type == TokenType::MUL)
        {
            begin = getNextToken();
            ret->children.push_back(pointer(begin));
        }
        else if (typeQualifier(begin))
        {
            begin = getNextToken();
            ret->children.push_back(typeQualifierList(begin));
            if (peekNextToken()->type == TokenType::MUL)
            {
                begin = getNextToken();
                ret->children.push_back(pointer(begin));
            }
        }
    }

    return ret;
}
std::shared_ptr<Node> AST::typeQualifierList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::TYPE_QUALIFIER_LIST);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    while (typeQualifier(begin))
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
    }
    ungetToken();
    return ret;
}
std::shared_ptr<Node> AST::specifierQualifierList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::SPECIFIER_QUALIFIER_LIST);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    while (typeQualifier(begin) || typeSpecifier(begin))
    {
        if (structUnion(begin))
            ret->children.push_back((structUnionSpecifier(begin)));
        else if (begin->type == TokenType::ENUM)
        {
            ret->children.push_back(enumSpecifier(begin));
        }
        else
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
    }
    ungetToken();
    return ret;
}
std::shared_ptr<Node> AST::enumSpecifier(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::ENUM_SPECIFIER);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    if (begin->type == TokenType::ENUM)
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    begin = getNextToken();
    if (begin->type == TokenType::L_CUR)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = getNextToken();
        ret->children.push_back(enumeratorList(begin));
        begin = getNextToken();
        if (begin->type == TokenType::R_CUR)
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
    }
    if (begin->type == TokenType::ID)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = peekNextToken();
        if (begin->type == TokenType::L_CUR)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = getNextToken();
            ret->children.push_back(enumeratorList(begin));
            begin = getNextToken();
            if (begin->type == TokenType::R_CUR)
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        }
    }
    return ret;
}
std::shared_ptr<Node> AST::enumerator(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::ENUMERATOR);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    if (begin->type == TokenType::ID)
    {
        ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
        begin = peekNextToken();
        if (begin->type == TokenType::ASSIGN)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = peekNextToken();
            if (begin->type == TokenType::CONSTANT)
            {
                begin = getNextToken();
                ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            }
        }
    }
    return ret;
}
std::shared_ptr<Node> AST::enumeratorList(std::list<Token>::iterator begin)
{
    Node stmt(TokenType::ENUMERATOR_LIST);
    std::shared_ptr<Node> ret;
    ret = std::make_shared<Node>(std::move(stmt));
    if (begin->type == TokenType::ID)
    {
        ret->children.push_back(enumerator(begin));
        while (peekNextToken()->type == TokenType::COMMA)
        {
            begin = getNextToken();
            ret->children.push_back(std::make_shared<Node>(std::move(*begin)));
            begin = peekNextToken();
            if (begin->type == TokenType::ID)
            {
                begin = getNextToken();
                ret->children.push_back(enumerator(begin));
            }
        }
    }
    return ret;
}
void AST::printBranches(const std::vector<bool> &isLast, std::ostream &os)
{
    for (size_t i = 0; i < isLast.size() - 1; ++i)
    {
        if (!isLast[i])
        {
            os << "│   ";
        }
        else
        {
            os << "    ";
        }
    }
}

void AST::traverseTree(std::shared_ptr<Node> node, std::vector<bool> &isLast, std::ostream &os)
{
    if (!node)
        return;

    // Print branches for current level
    if (!isLast.empty())
    {
        printBranches(isLast, os);
        if (isLast.back())
        {
            os << "└── ";
        }
        else
        {
            os << "├── ";
        }
    }
    os << "Token: " << t(node->t.type) << " ";
    os << "lexeme: " << node->t.lexeme << std::endl;

    // Process children
    for (size_t i = 0; i < node->children.size(); ++i)
    {
        isLast.push_back(i == node->children.size() - 1);
        traverseTree(node->children[i], isLast, os);
        isLast.pop_back();
    }
}
void AST::printAST(std::ostream &os)
{

    if (!root)
        return;
    std::vector<bool> isLast;
    traverseTree(root, isLast, os);
}
