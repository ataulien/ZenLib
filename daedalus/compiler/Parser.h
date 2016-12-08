/*
 * Copyright (C) 2016   Hedede <haddayn@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 */
#ifndef Daedalus_Parser
#define Daedalus_Parser
#include <daedalus/compiler/Lexer.h>
#include <daedalus/compiler/Type.h>
#include <daedalus/compiler/Declaration.h>
#include <daedalus/compiler/Operators.h>
namespace Daedalus {
namespace tree {
class Statement;
class StatementBlock;
class FunctionProto;
class Variable;
class Prototype;
class Instance;
class Expression;
} // namespace tree

class SymbolTable;
class DiagnosticHelper;

/*!
 * Parser transforms token stream into the Abstract Syntax Tree
 */
struct Parser {
	Parser(Lexer& lexer, DiagnosticHelper& diag, SymbolTable& symtab)
		: lexer(lexer), diag(diag), symtab(symtab)
	{
		token = lexer.currentToken();
	}

	~Parser() = default;

	/*!
	 * Parse a top-level declaration.
	 */
	std::unique_ptr<tree::Declaration> parseDeclaration();

	/*!
	 * Consume peek token and extract next token.
	 */
	Token getNextToken()
	{
		return token = lexer.nextToken();
	}

private:
	/*! Current lookahead (peek) token. */
	Token token;

	/*! Lexer which provides the stream of tokens */
	Lexer& lexer;

	/*! Diagnostics helper for error reporting */
	DiagnosticHelper& diag;

	/*! Parser uses Symbol Table for semantic analysis */
	SymbolTable& symtab;

	/*!
	 * Match token, and consume if matched.
	 */
	bool match(Token::Kind expected);

	Class* readType();

	std::unique_ptr<tree::FunctionProto> parseFunctionPrototype();
	std::unique_ptr<tree::Declaration> parseFunctionDefinition();
	std::unique_ptr<tree::Declaration> parseClass();
	std::unique_ptr<tree::Prototype> parsePrototype();
	std::unique_ptr<tree::Instance> parseInstance();
	std::unique_ptr<tree::Variable> parseVariable(Type type);
	std::unique_ptr<tree::Declaration> parseGlobalVar();
	std::unique_ptr<tree::Declaration> parseLocalVar();
	std::unique_ptr<tree::Declaration> parseConstant();
	std::unique_ptr<tree::Statement> parseLocal();

	std::unique_ptr<tree::StatementBlock> parseStatementBlock();
	std::unique_ptr<tree::Statement> parseStatement();
	std::unique_ptr<tree::Statement> parseBranchStatement();
	std::unique_ptr<tree::Statement> parseWhileStatement();
	std::unique_ptr<tree::Statement> parseDoStatement();
	std::unique_ptr<tree::Statement> parseBreakStatement();
	std::unique_ptr<tree::Statement> parseReturnStatement();
	std::unique_ptr<tree::Statement> parseExprStatement();

	std::unique_ptr<tree::Expression> parseExpression();

	std::unique_ptr<tree::Expression> parseArrayInitializer();

	std::unique_ptr<tree::Expression> parseUnaryExpr();
	std::unique_ptr<tree::Expression> parseBinaryExpr(
	                std::unique_ptr<tree::Expression> LHS,
	                prec::Level minPrec);

	std::unique_ptr<tree::Expression> parsePrimaryExpr();
	std::unique_ptr<tree::Expression> parseIdentifierExpr();
	std::unique_ptr<tree::Expression> parseParenExpr();
	std::unique_ptr<tree::Expression> parseNumberExpr();
	std::unique_ptr<tree::Expression> parseStringExpr();
	std::unique_ptr<tree::Expression> parseCallExpr(std::string func);
	std::unique_ptr<tree::Expression> parseFieldAccess(std::string id);
	std::unique_ptr<tree::Expression> parseArraySubscript(std::unique_ptr<tree::Expression>);
};
} // namespace daedalus
#endif //Daedalus_Parser
