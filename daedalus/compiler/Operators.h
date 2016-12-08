/*
 * Copyright (C) 2016 Hedede <haddayn@gmail.com>
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
#ifndef Daedalus_OperatorPrecedence
#define Daedalus_OperatorPrecedence
#include <daedalus/compiler/Token.h>
namespace Daedalus {
namespace prec {
enum Level {
	None           = -1,
	Unknown        = 0,
	Assignment     = 1,  // =, -=, +=, *=, /=
	LogicalOr      = 2,
	LogicalAnd     = 3,
	BitwiseOr      = 4,
	BitwiseXor     = 5,
	BitwiseAnd     = 6,
	Equality       = 7,  // ==, !=
	Relational     = 8,  // >=, <=, <, >
	Shift          = 9,  // <<, >>
	Additive       = 10, // +, -
	Multiplicative = 11  // *, *, %
};
} // namespace prec

prec::Level getOperatorPrecedence(Token tok)
{
	switch(tok.type()) {
	default:
		// Not an operator
		return prec::None;
	case Token::equal:
	case Token::ast_equal:
	case Token::plus_equal:
	case Token::minus_equal:
	case Token::slash_equal:
		return prec::Assignment;
	case Token::pipe_pipe:
		return prec::LogicalOr;
	case Token::amp_amp:
		return prec::LogicalAnd;
	case Token::pipe:
		return prec::BitwiseOr;
	case Token::caret:
		return prec::BitwiseXor;
	case Token::amp:
		return prec::BitwiseAnd;
	case Token::equal_equal:
	case Token::bang_equal:
		return prec::Equality;
	case Token::less:
	case Token::less_equal:
	case Token::greater:
	case Token::greater_equal:
		return prec::Relational;
	case Token::less_less:
	case Token::greater_greater:
		return prec::Shift;
	case Token::plus:
	case Token::minus:
		return prec::Additive;
	case Token::ast:
	case Token::slash:
	case Token::percent:
		return prec::Multiplicative;	
	}
}

bool isRightAssociative(Token tok)
{
	return getOperatorPrecedence(tok) == prec::Assignment;
}

bool isBinaryOperator(Token tok)
{
	return getOperatorPrecedence(tok) > prec::Unknown;
}

bool isUnaryOperator(Token tok)
{
	switch (tok.type()) {
	case Token::bang:
	case Token::tilde:
	case Token::plus:
	case Token::minus:
		return true;
	default:
		return false;
	}
}

bool isOperator(Token tok)
{
	return isBinaryOperator(tok) || isUnaryOperator(tok);
}
} // namespace daedalus
#endif //Daedalus_OperatorPrecedence
