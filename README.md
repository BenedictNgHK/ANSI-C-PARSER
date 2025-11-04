
# ANSI C Parser

A pure C++ implementation of an ANSI C parser without any third-party dependencies. This project implements a complete parser for the ANSI C programming language that can tokenize, parse, and generate an Abstract Syntax Tree (AST) from C source code.

## Features

- Complete ANSI C grammar support
- Pure C++ implementation
- No external dependencies
- Generates detailed AST output
- Supports structs, functions, and type definitions
- Handles includes and basic preprocessor directives
- Token-level line number tracking

## Getting Started

### Prerequisites

- C++ compiler with C++11 support
- Make build system

### Building

To build the project, simply run:

```bash
make
```

### Running

To run the parser with the example file:

```bash
./run.sh
```

## Project Structure

- `AST.cpp/hpp` - Abstract Syntax Tree implementation
- `Error.cpp/hpp` - Error handling utilities
- `Scanner.cpp/hpp` - Lexical analyzer/scanner
- `Token.cpp/hpp` - Token definitions and handling
- `grammar.y` - ANSI C grammar definition
- `main.cpp` - Main program entry point

## Grammar Reference

The parser implements the ANSI C grammar as defined in:
[ANSI C Grammar Specification](https://www.quut.com/c/ANSI-C-grammar-y-2011.html)

## Example Output

The parser generates a detailed token stream and AST. Here's a sample output:

```
INCLUDE
Type: LT Lexeme: 
line NO: 2
Type: INCLUDE_PATH Lexeme: stdio.h
line NO: 3
Type: GT Lexeme: 
line NO: 4
Type: STRUCT Lexeme: struct
Type: ID Lexeme: Person
Type: L_CUR Lexeme: {
Type: CHAR_TYPE Lexeme: char
Type: ID Lexeme: name
Type: L_SQR Lexeme: [
line NO: 5
Type: CONSTANT Lexeme: 20
Type: R_SQR Lexeme: ]
Type: SEMI_COLON Lexeme: ;
line NO: 6
Type: INT_TYPE Lexeme: int
Type: ID Lexeme: age
line NO: 7
Type: SEMI_COLON Lexeme: ;
Type: R_CUR Lexeme: }
Type: SEMI_COLON Lexeme: ;
Type: INT_TYPE Lexeme: int
Type: ID Lexeme: add
Type: L_BR Lexeme: (
Type: INT_TYPE Lexeme: int
Type: ID Lexeme: a
Type: COMMA Lexeme: ,
line NO: 8
Type: INT_TYPE Lexeme: int
line NO: 9
Type: ID Lexeme: b
Type: R_BR Lexeme: )
Type: L_CUR Lexeme: {
Type: RETURN Lexeme: return
Type: ID Lexeme: a
line NO: 10
Type: PLUS Lexeme: +
line NO: 11
Type: ID Lexeme: b
Type: SEMI_COLON Lexeme: ;
Type: R_CUR Lexeme: }
Type: TYPEDEF Lexeme: typedef
Type: STRUCT Lexeme: struct
line NO: 12
Type: ID Lexeme: Person
Type: ID Lexeme: Person
Type: SEMI_COLON Lexeme: ;
Type: ID Lexeme: Person
Type: ID Lexeme: p
Type: ASSIGN Lexeme: =
Type: L_CUR Lexeme: {
Type: DOT Lexeme: .
Type: ID Lexeme: name
Type: ASSIGN Lexeme: =
Type: STRING_LITERAL Lexeme: John
Type: COMMA Lexeme: ,
Type: DOT Lexeme: .
Type: ID Lexeme: age
Type: ASSIGN Lexeme: =
line NO: 13
Type: CONSTANT Lexeme: 20
Type: R_CUR Lexeme: }
Type: SEMI_COLON Lexeme: ;
Type: INT_TYPE Lexeme: int
line NO: 15
Type: MAIN Lexeme: main
line NO: 16
Type: L_BR Lexeme: (
Type: R_BR Lexeme: )
Type: L_CUR Lexeme: {
Type: ID Lexeme: printf
Type: L_BR Lexeme: (
line NO: 17
Type: STRING_LITERAL Lexeme: Hello, World!

Type: R_BR Lexeme: )
Type: SEMI_COLON Lexeme: ;
line NO: 18
Type: RETURN Lexeme: return
line NO: 19
Type: CONSTANT Lexeme: 0
Type: SEMI_COLON Lexeme: ;
Type: R_CUR Lexeme: }
Token: TRANSLATION_UNIT lexeme: Hello.c
├── Token: INCLUDE_STMT lexeme: 
│   ├── Token: LT lexeme: 
│   ├── Token: INCLUDE_PATH lexeme: stdio.h
│   └── Token: GT lexeme: 
├── Token: EXTERNAL_DECLARATION lexeme: 
│   └── Token: DECLARATION lexeme: 
│       ├── Token: DECLARATION_SPECIFIERS lexeme: 
│       │   └── Token: STRUCT_UNION_SPECIFIER lexeme: 
│       │       ├── Token: STRUCT lexeme: struct
│       │       ├── Token: ID lexeme: Person
│       │       ├── Token: L_CUR lexeme: {
│       │       ├── Token: STRUCT_DECLARATION_LIST lexeme: 
│       │       │   ├── Token: STRUCT_DECLARATION lexeme: 
│       │       │   │   ├── Token: SPECIFIER_QUALIFIER_LIST lexeme: 
│       │       │   │   │   └── Token: CHAR_TYPE lexeme: char
│       │       │   │   ├── Token: STRUCT_DECLARATOR_LIST lexeme: 
│       │       │   │   │   └── Token: STRUCT_DECLARATOR lexeme: 
│       │       │   │   │       └── Token: DECLARATOR lexeme: 
│       │       │   │   │           └── Token: DIRECT_DECLARATOR lexeme: 
│       │       │   │   │               ├── Token: ID lexeme: name
│       │       │   │   │               ├── Token: L_SQR lexeme: [
│       │       │   │   │               ├── Token: ASSIGNMENT_EXPRESSION lexeme: 
│       │       │   │   │               │   └── Token: CONDITIONAL_EXPRESSION lexeme: 
│       │       │   │   │               │       └── Token: LOGICAL_OR_EXPRESSION lexeme: 
│       │       │   │   │               │           └── Token: LOGICAL_AND_EXPRESSION lexeme: 
│       │       │   │   │               │               └── Token: INCLUSIVE_OR_EXPRESSION lexeme: 
│       │       │   │   │               │                   └── Token: EXCLUSIVE_OR_EXPRESSION lexeme: 
│       │       │   │   │               │                       └── Token: AND_EXPRESSION lexeme: 
│       │       │   │   │               │                           └── Token: EQUALITY_EXPRESSION lexeme: 
│       │       │   │   │               │                               └── Token: RELATIONAL_EXPRESSION lexeme: 
│       │       │   │   │               │                                   └── Token: SHIFT_EXPRESSION lexeme: 
│       │       │   │   │               │                                       └── Token: ADDITIVE_EXPRESSION lexeme: 
│       │       │   │   │               │                                           └── Token: MULTIPLICATIVE_EXPRESSION lexeme: 
│       │       │   │   │               │                                               └── Token: CAST_EXPRESSION lexeme: 
│       │       │   │   │               │                                                   └── Token: UNARY_EXPRESSION lexeme: 
│       │       │   │   │               │                                                       └── Token: POSTFIX_EXPRESSION lexeme: 
│       │       │   │   │               │                                                           └── Token: PRIMARY_EXPRESSION lexeme: 
│       │       │   │   │               │                                                               └── Token: CONSTANT lexeme: 20
│       │       │   │   │               └── Token: R_SQR lexeme: ]
│       │       │   │   └── Token: SEMI_COLON lexeme: ;
│       │       │   └── Token: STRUCT_DECLARATION lexeme: 
│       │       │       ├── Token: SPECIFIER_QUALIFIER_LIST lexeme: 
│       │       │       │   └── Token: INT_TYPE lexeme: int
│       │       │       ├── Token: STRUCT_DECLARATOR_LIST lexeme: 
│       │       │       │   └── Token: STRUCT_DECLARATOR lexeme: 
│       │       │       │       └── Token: DECLARATOR lexeme: 
│       │       │       │           └── Token: DIRECT_DECLARATOR lexeme: 
│       │       │       │               └── Token: ID lexeme: age
│       │       │       └── Token: SEMI_COLON lexeme: ;
│       │       └── Token: R_CUR lexeme: }
│       └── Token: SEMI_COLON lexeme: ;
├── Token: EXTERNAL_DECLARATION lexeme: 
│   └── Token: FUNCTION_DEFINITION lexeme: 
│       ├── Token: DECLARATION_SPECIFIERS lexeme: 
│       │   └── Token: INT_TYPE lexeme: int
│       ├── Token: DECLARATOR lexeme: 
│       │   └── Token: DIRECT_DECLARATOR lexeme: 
│       │       ├── Token: ID lexeme: add
│       │       ├── Token: L_BR lexeme: (
│       │       ├── Token: PARAMETER_TYPE_LIST lexeme: 
│       │       │   └── Token: PARAMETER_LIST lexeme: 
│       │       │       ├── Token: PARAMETER_DECLARATION lexeme: 
│       │       │       │   ├── Token: DECLARATION_SPECIFIERS lexeme: 
│       │       │       │   │   └── Token: INT_TYPE lexeme: int
│       │       │       │   └── Token: DECLARATOR lexeme: 
│       │       │       │       └── Token: DIRECT_DECLARATOR lexeme: 
│       │       │       │           └── Token: ID lexeme: a
│       │       │       ├── Token: COMMA lexeme: ,
│       │       │       └── Token: PARAMETER_DECLARATION lexeme: 
│       │       │           ├── Token: DECLARATION_SPECIFIERS lexeme: 
│       │       │           │   └── Token: INT_TYPE lexeme: int
│       │       │           └── Token: DECLARATOR lexeme: 
│       │       │               └── Token: DIRECT_DECLARATOR lexeme: 
│       │       │                   └── Token: ID lexeme: b
│       │       └── Token: R_BR lexeme: )
│       └── Token: COMPOUND_STATEMENT lexeme: 
│           ├── Token: L_CUR lexeme: {
│           ├── Token: BLOCK_ITEM_LIST lexeme: 
│           │   └── Token: BLOCK_ITEM lexeme: 
│           │       └── Token: STATEMENT lexeme: 
│           │           └── Token: JUMP_STATEMENT lexeme: 
│           │               ├── Token: RETURN lexeme: return
│           │               ├── Token: EXPRESSION lexeme: 
│           │               │   └── Token: ASSIGNMENT_EXPRESSION lexeme: 
│           │               │       └── Token: CONDITIONAL_EXPRESSION lexeme: 
│           │               │           └── Token: LOGICAL_OR_EXPRESSION lexeme: 
│           │               │               └── Token: LOGICAL_AND_EXPRESSION lexeme: 
│           │               │                   └── Token: INCLUSIVE_OR_EXPRESSION lexeme: 
│           │               │                       └── Token: EXCLUSIVE_OR_EXPRESSION lexeme: 
│           │               │                           └── Token: AND_EXPRESSION lexeme: 
│           │               │                               └── Token: EQUALITY_EXPRESSION lexeme: 
│           │               │                                   └── Token: RELATIONAL_EXPRESSION lexeme: 
│           │               │                                       └── Token: SHIFT_EXPRESSION lexeme: 
│           │               │                                           └── Token: ADDITIVE_EXPRESSION lexeme: 
│           │               │                                               ├── Token: MULTIPLICATIVE_EXPRESSION lexeme: 
│           │               │                                               │   └── Token: CAST_EXPRESSION lexeme: 
│           │               │                                               │       └── Token: UNARY_EXPRESSION lexeme: 
│           │               │                                               │           └── Token: POSTFIX_EXPRESSION lexeme: 
│           │               │                                               │               └── Token: PRIMARY_EXPRESSION lexeme: 
│           │               │                                               │                   └── Token: ID lexeme: a
│           │               │                                               ├── Token: PLUS lexeme: +
│           │               │                                               └── Token: MULTIPLICATIVE_EXPRESSION lexeme: 
│           │               │                                                   └── Token: CAST_EXPRESSION lexeme: 
│           │               │                                                       └── Token: UNARY_EXPRESSION lexeme: 
│           │               │                                                           └── Token: POSTFIX_EXPRESSION lexeme: 
│           │               │                                                               └── Token: PRIMARY_EXPRESSION lexeme: 
│           │               │                                                                   └── Token: ID lexeme: b
│           │               └── Token: SEMI_COLON lexeme: ;
│           └── Token: R_CUR lexeme: }
├── Token: EXTERNAL_DECLARATION lexeme: 
│   └── Token: DECLARATION lexeme: 
│       ├── Token: DECLARATION_SPECIFIERS lexeme: 
│       │   ├── Token: TYPEDEF lexeme: typedef
│       │   └── Token: STRUCT_UNION_SPECIFIER lexeme: 
│       │       ├── Token: STRUCT lexeme: struct
│       │       └── Token: ID lexeme: Person
│       ├── Token: INIT_DECLARATOR_LIST lexeme: 
│       │   └── Token: INIT_DECLARATOR lexeme: 
│       │       └── Token: DECLARATOR lexeme: 
│       │           └── Token: DIRECT_DECLARATOR lexeme: 
│       │               └── Token: ID lexeme: Person
│       └── Token: SEMI_COLON lexeme: ;
├── Token: EXTERNAL_DECLARATION lexeme: 
│   └── Token: DECLARATION lexeme: 
│       ├── Token: DECLARATION_SPECIFIERS lexeme: 
│       │   └── Token: ID lexeme: Person
│       ├── Token: INIT_DECLARATOR_LIST lexeme: 
│       │   └── Token: INIT_DECLARATOR lexeme: 
│       │       ├── Token: DECLARATOR lexeme: 
│       │       │   └── Token: DIRECT_DECLARATOR lexeme: 
│       │       │       └── Token: ID lexeme: p
│       │       ├── Token: ASSIGN lexeme: =
│       │       └── Token: INITIALIZER lexeme: 
│       │           ├── Token: L_CUR lexeme: {
│       │           ├── Token: INITIALIZER_LIST lexeme: 
│       │           │   ├── Token: DESIGNATION lexeme: 
│       │           │   │   ├── Token: DESIGNATOR_LIST lexeme: 
│       │           │   │   │   └── Token: DESIGNATOR lexeme: 
│       │           │   │   │       ├── Token: DOT lexeme: .
│       │           │   │   │       └── Token: ID lexeme: name
│       │           │   │   └── Token: ASSIGN lexeme: =
│       │           │   ├── Token: INITIALIZER lexeme: 
│       │           │   │   └── Token: ASSIGNMENT_EXPRESSION lexeme: 
│       │           │   │       └── Token: CONDITIONAL_EXPRESSION lexeme: 
│       │           │   │           └── Token: LOGICAL_OR_EXPRESSION lexeme: 
│       │           │   │               └── Token: LOGICAL_AND_EXPRESSION lexeme: 
│       │           │   │                   └── Token: INCLUSIVE_OR_EXPRESSION lexeme: 
│       │           │   │                       └── Token: EXCLUSIVE_OR_EXPRESSION lexeme: 
│       │           │   │                           └── Token: AND_EXPRESSION lexeme: 
│       │           │   │                               └── Token: EQUALITY_EXPRESSION lexeme: 
│       │           │   │                                   └── Token: RELATIONAL_EXPRESSION lexeme: 
│       │           │   │                                       └── Token: SHIFT_EXPRESSION lexeme: 
│       │           │   │                                           └── Token: ADDITIVE_EXPRESSION lexeme: 
│       │           │   │                                               └── Token: MULTIPLICATIVE_EXPRESSION lexeme: 
│       │           │   │                                                   └── Token: CAST_EXPRESSION lexeme: 
│       │           │   │                                                       └── Token: UNARY_EXPRESSION lexeme: 
│       │           │   │                                                           └── Token: POSTFIX_EXPRESSION lexeme: 
│       │           │   │                                                               └── Token: PRIMARY_EXPRESSION lexeme: 
│       │           │   │                                                                   └── Token: STRING_LITERAL lexeme: John
│       │           │   ├── Token: COMMA lexeme: ,
│       │           │   ├── Token: DESIGNATION lexeme: 
│       │           │   │   ├── Token: DESIGNATOR_LIST lexeme: 
│       │           │   │   │   └── Token: DESIGNATOR lexeme: 
│       │           │   │   │       ├── Token: DOT lexeme: .
│       │           │   │   │       └── Token: ID lexeme: age
│       │           │   │   └── Token: ASSIGN lexeme: =
│       │           │   └── Token: INITIALIZER lexeme: 
│       │           │       └── Token: ASSIGNMENT_EXPRESSION lexeme: 
│       │           │           └── Token: CONDITIONAL_EXPRESSION lexeme: 
│       │           │               └── Token: LOGICAL_OR_EXPRESSION lexeme: 
│       │           │                   └── Token: LOGICAL_AND_EXPRESSION lexeme: 
│       │           │                       └── Token: INCLUSIVE_OR_EXPRESSION lexeme: 
│       │           │                           └── Token: EXCLUSIVE_OR_EXPRESSION lexeme: 
│       │           │                               └── Token: AND_EXPRESSION lexeme: 
│       │           │                                   └── Token: EQUALITY_EXPRESSION lexeme: 
│       │           │                                       └── Token: RELATIONAL_EXPRESSION lexeme: 
│       │           │                                           └── Token: SHIFT_EXPRESSION lexeme: 
│       │           │                                               └── Token: ADDITIVE_EXPRESSION lexeme: 
│       │           │                                                   └── Token: MULTIPLICATIVE_EXPRESSION lexeme: 
│       │           │                                                       └── Token: CAST_EXPRESSION lexeme: 
│       │           │                                                           └── Token: UNARY_EXPRESSION lexeme: 
│       │           │                                                               └── Token: POSTFIX_EXPRESSION lexeme: 
│       │           │                                                                   └── Token: PRIMARY_EXPRESSION lexeme: 
│       │           │                                                                       └── Token: CONSTANT lexeme: 20
│       │           └── Token: R_CUR lexeme: }
│       └── Token: SEMI_COLON lexeme: ;
└── Token: EXTERNAL_DECLARATION lexeme: 
    └── Token: FUNCTION_DEFINITION lexeme: 
        ├── Token: DECLARATION_SPECIFIERS lexeme: 
        │   └── Token: INT_TYPE lexeme: int
        ├── Token: DECLARATOR lexeme: 
        │   └── Token: DIRECT_DECLARATOR lexeme: 
        │       ├── Token: MAIN lexeme: main
        │       ├── Token: L_BR lexeme: (
        │       └── Token: R_BR lexeme: )
        └── Token: COMPOUND_STATEMENT lexeme: 
            ├── Token: L_CUR lexeme: {
            ├── Token: BLOCK_ITEM_LIST lexeme: 
            │   ├── Token: BLOCK_ITEM lexeme: 
            │   │   └── Token: STATEMENT lexeme: 
            │   │       └── Token: EXPRESSION_STATEMENT lexeme: 
            │   │           ├── Token: EXPRESSION lexeme: 
            │   │           │   └── Token: ASSIGNMENT_EXPRESSION lexeme: 
            │   │           │       └── Token: CONDITIONAL_EXPRESSION lexeme: 
            │   │           │           └── Token: LOGICAL_OR_EXPRESSION lexeme: 
            │   │           │               └── Token: LOGICAL_AND_EXPRESSION lexeme: 
            │   │           │                   └── Token: INCLUSIVE_OR_EXPRESSION lexeme: 
            │   │           │                       └── Token: EXCLUSIVE_OR_EXPRESSION lexeme: 
            │   │           │                           └── Token: AND_EXPRESSION lexeme: 
            │   │           │                               └── Token: EQUALITY_EXPRESSION lexeme: 
            │   │           │                                   └── Token: RELATIONAL_EXPRESSION lexeme: 
            │   │           │                                       └── Token: SHIFT_EXPRESSION lexeme: 
            │   │           │                                           └── Token: ADDITIVE_EXPRESSION lexeme: 
            │   │           │                                               └── Token: MULTIPLICATIVE_EXPRESSION lexeme: 
            │   │           │                                                   └── Token: CAST_EXPRESSION lexeme: 
            │   │           │                                                       └── Token: UNARY_EXPRESSION lexeme: 
            │   │           │                                                           └── Token: POSTFIX_EXPRESSION lexeme: 
            │   │           │                                                               ├── Token: PRIMARY_EXPRESSION lexeme: 
            │   │           │                                                               │   └── Token: ID lexeme: printf
            │   │           │                                                               ├── Token: L_BR lexeme: (
            │   │           │                                                               ├── Token: ARGUMENT_EXPRESSION_LIST lexeme: 
            │   │           │                                                               │   └── Token: ASSIGNMENT_EXPRESSION lexeme: 
            │   │           │                                                               │       └── Token: CONDITIONAL_EXPRESSION lexeme: 
            │   │           │                                                               │           └── Token: LOGICAL_OR_EXPRESSION lexeme: 
            │   │           │                                                               │               └── Token: LOGICAL_AND_EXPRESSION lexeme: 
            │   │           │                                                               │                   └── Token: INCLUSIVE_OR_EXPRESSION lexeme: 
            │   │           │                                                               │                       └── Token: EXCLUSIVE_OR_EXPRESSION lexeme: 
            │   │           │                                                               │                           └── Token: AND_EXPRESSION lexeme: 
            │   │           │                                                               │                               └── Token: EQUALITY_EXPRESSION lexeme: 
            │   │           │                                                               │                                   └── Token: RELATIONAL_EXPRESSION lexeme: 
            │   │           │                                                               │                                       └── Token: SHIFT_EXPRESSION lexeme: 
            │   │           │                                                               │                                           └── Token: ADDITIVE_EXPRESSION lexeme: 
            │   │           │                                                               │                                               └── Token: MULTIPLICATIVE_EXPRESSION lexeme: 
            │   │           │                                                               │                                                   └── Token: CAST_EXPRESSION lexeme: 
            │   │           │                                                               │                                                       └── Token: UNARY_EXPRESSION lexeme: 
            │   │           │                                                               │                                                           └── Token: POSTFIX_EXPRESSION lexeme: 
            │   │           │                                                               │                                                               └── Token: PRIMARY_EXPRESSION lexeme: 
            │   │           │                                                               │                                                                   └── Token: STRING_LITERAL lexeme: Hello, World!

            │   │           │                                                               └── Token: R_BR lexeme: )
            │   │           └── Token: SEMI_COLON lexeme: ;
            │   └── Token: BLOCK_ITEM lexeme: 
            │       └── Token: STATEMENT lexeme: 
            │           └── Token: JUMP_STATEMENT lexeme: 
            │               ├── Token: RETURN lexeme: return
            │               ├── Token: EXPRESSION lexeme: 
            │               │   └── Token: ASSIGNMENT_EXPRESSION lexeme: 
            │               │       └── Token: CONDITIONAL_EXPRESSION lexeme: 
            │               │           └── Token: LOGICAL_OR_EXPRESSION lexeme: 
            │               │               └── Token: LOGICAL_AND_EXPRESSION lexeme: 
            │               │                   └── Token: INCLUSIVE_OR_EXPRESSION lexeme: 
            │               │                       └── Token: EXCLUSIVE_OR_EXPRESSION lexeme: 
            │               │                           └── Token: AND_EXPRESSION lexeme: 
            │               │                               └── Token: EQUALITY_EXPRESSION lexeme: 
            │               │                                   └── Token: RELATIONAL_EXPRESSION lexeme: 
            │               │                                       └── Token: SHIFT_EXPRESSION lexeme: 
            │               │                                           └── Token: ADDITIVE_EXPRESSION lexeme: 
            │               │                                               └── Token: MULTIPLICATIVE_EXPRESSION lexeme: 
            │               │                                                   └── Token: CAST_EXPRESSION lexeme: 
            │               │                                                       └── Token: UNARY_EXPRESSION lexeme: 
            │               │                                                           └── Token: POSTFIX_EXPRESSION lexeme: 
            │               │                                                               └── Token: PRIMARY_EXPRESSION lexeme: 
            │               │                                                                   └── Token: CONSTANT lexeme: 0
            │               └── Token: SEMI_COLON lexeme: ;
            └── Token: R_CUR lexeme: }
```