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
#include <algorithm>
#include <daedalus/compiler/Lexer.h>
#include "CharType.h"
namespace Daedalus {
namespace {
bool in (char c, char c1)
{
	return c == c1;
}

template<typename... Args>
bool in (char c, char c1, Args... chars)
{
	return c == c1 || in(c, chars...);
}
} // namespace

constexpr bool EnableWhileStatements = false;
Lexer::Lexer(SourceBuffer* inputBuffer)
	: buf(inputBuffer)
{
	// Setup keywords
	kwmap
	.add("const", Token::kw_const)
	.add("var",   Token::kw_var)
	.add("func",  Token::kw_func)
	.add("if",    Token::kw_if)
	.add("else",  Token::kw_else)
	.add("class", Token::kw_class)
	.add("prototype", Token::kw_prototype)
	.add("instance",  Token::kw_instance)
	.add("return", Token::kw_return)
	.add("void",   Token::kw_void)
	.add("float",  Token::kw_float)
	.add("int",    Token::kw_int)
	.add("string", Token::kw_string);

	if (EnableWhileStatements)
	kwmap
	.add("while",  Token::kw_while)
	.add("do",     Token::kw_do)
	.add("extern", Token::kw_extern)
	.add("break",  Token::kw_break)
	.add("continue", Token::kw_continue);

	cur = buf->begin();
	end = buf->end();

	// Extract first token
	nextToken();
}

void Lexer::init()
{
}

Token Lexer::currentToken()
{
	return cur_token;
}

/*!
 * Extract next token from stream
 */
Token Lexer::nextToken()
{
	lexNextToken(cur_token);
	return currentToken();
}

char Lexer::peek() const
{
	return (cur < end) ? *(cur + 1) : 0;
}

char Lexer::prev() const
{
	assert(cur > std::begin(*buf));
	return *(cur - 1);
}

bool Lexer::lexIdentifier(Token& token)
{
	char const* start = cur;
	cur = std::find_if_not(cur, end, isNameChar);

	std::string id(start, cur);

	// Check if token is a reserved keyword
	auto kind = kwmap.get(id);

	if (!kind)
		kind = Token::identifier;

	token.setType(kind);
	token.setData(start, cur);
	token.setLocation(size_t(start - buf->begin()));
	
	return true;
}

bool Lexer::lexNumericConstant(Token& token)
{
	char const* start = cur;

	cur = std::find_if_not(cur, end, isalnum);

	if (*cur == '.') {
		cur = std::find_if_not(cur+1, end, isalnum);

		if (in(*cur, '-', '+') && in(prev(), 'e', 'E'))
			cur = std::find_if_not(cur, end, isalnum);
	}

	token.setData(start, cur);
	token.setType(Token::numeric_constant);
	token.setLocation(size_t(start - buf->begin()));

	return true;
}

bool Lexer::lexStringLiteral(Token& token)
{
	char const* start = cur;
	while (*cur != '"') {
		cur = std::find(cur, end, '"');
		if (cur == end)
			break;
		if (prev() == '\\')
			++cur;
	}

	// TODO: incomplete flag if cur == end
	token.setData(start, cur);
	token.setType(Token::string_literal);
	token.setLocation(size_t(start - buf->begin()));

	if (cur < end)
		++cur; // consume '"'
	return true;
}

bool Lexer::lexIllegalToken(Token& token)
{
	char const* begin = cur;
	// TODO: search until token-beginnning character
	cur = std::find_if(begin, end, isspace);

	token.setData(begin, cur);
	token.setType(Token::illegal);
	token.setLocation(size_t(begin - buf->begin()));

	return true;
}

void Lexer::skipBlockComment()
{
	// Skip ‘/* ’.
	// Third character:
	// - If it is '/', then it needs to be skipped or ‘/*/’
	// would be handled incorrectly
	// - If it is anything else, then it doesn't matter and
	// ca be skipped in any case.
	cur += 3;

	while (true) {
		cur = std::find(cur, end, '/');

		if (cur == end)
			break;

		if (prev() == '*')
			break;
		++cur;
	}
}

void Lexer::handleComment()
{
	char p = peek();
	if (p == '*') {
		/* Skip block comment */
		skipBlockComment();
	} else if (p == '/') {
		// Skip line comment
		cur = std::find(cur, end, '\n');
	} else {
		// Not a comment - we're done.
		return;
	}
	
	// Instead of going through everything again, we do everything here.
	// Skip whitespace and check for more comments
	cur = std::find_if_not(cur+1, end, isspace);

	if (*cur == '/')
		handleComment();
}

/*!
 * This function is an actual lexer implementation.
 * It reads characters from stream and produces tokens
 */
bool Lexer::lexNextToken(Token& tok)
{
lexNextToken:
	cur = std::find_if_not(cur, end, isspace);

	char const* tok_start = cur;

	switch (*cur) {
	case 0:
		tok.setType(Token::eof);
		return true;
	/* Numeric constants */
	// case '0' ... '9'
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return lexNumericConstant(tok);
	/* String literal */
	case '"':
		++ cur; // consume '"'
		return lexStringLiteral(tok);
	/* Identifier */
	// case 'A' ... 'Z':
	// case 'a' ... 'z':
	// case '_':
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
	case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
	case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
	case 'V': case 'W': case 'X': case 'Y': case 'Z':
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
	case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
	case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
	case 'v': case 'w': case 'x': case 'y': case 'z':
	case '_':
		return lexIdentifier(tok);
	/* Operators */
	case '^':
		tok.setType(Token::caret);
		break;
	case '~':
		tok.setType(Token::tilde);
		break;
	case ',':
		tok.setType(Token::comma);
		break;
	case '.':
		tok.setType(Token::dot);
		break;
	case ';':
		tok.setType(Token::semicolon);
		break;
	case '%':
		tok.setType(Token::percent);
		break;
	case '{':
		tok.setType(Token::l_brace);
		break;
	case '}':
		tok.setType(Token::r_brace);
		break;
	case '[':
		tok.setType(Token::l_bracket);
		break;
	case ']':
		tok.setType(Token::r_bracket);
		break;
	case '(':
		tok.setType(Token::l_paren);
		break;
	case ')':
		tok.setType(Token::r_paren);
		break;
	case '&':
		if (peek() == '&') {
			tok.setType(Token::amp_amp);
			++cur;
		} else {
			tok.setType(Token::amp);
		}
		// TODO: does daedalus have '&=' operator?
		break;
	case '|':
		if (peek() == '|') {
			tok.setType(Token::pipe_pipe);
			++cur;
		} else {
			tok.setType(Token::pipe);
		}
		// TODO: does daedalus have '|= operator?
		break;
	case '!':
		if (peek() == '=') {
			tok.setType(Token::bang_equal);
			++cur;
		} else {
			tok.setType(Token::bang);
		}
		break;
	case '*':
		if (peek() == '=') {
			tok.setType(Token::ast_equal);
			++ cur;
		} else {
			tok.setType(Token::ast);
		}
		break;
	case '/':
		// Look for comments first
		handleComment();
		// Check what we have, after we're done with comments
		// If we have '/', continue handling this case.
		// If we have something different, restart lexer.
		if (*cur != '/')
			// We didn't lex anything, restart the lexer.
			goto lexNextToken;

		if (peek() == '=') {
			tok.setType(Token::slash_equal);
			++cur;
		} else {
			tok.setType(Token::slash);
		}
		break;
	case '=':
		if (peek() == '=') {
			tok.setType(Token::equal_equal);
			++cur;
		} else {
			tok.setType(Token::equal);
		}
		break;
	case '+':
		if (peek() == '+') {
			tok.setType(Token::plus_plus);
			++cur;
		} else if (peek() == '=') {
			tok.setType(Token::plus_equal);
			++cur;
		} else {
			tok.setType(Token::plus);
		}
		break;
	case '-':
		if (peek() == '-') {
			tok.setType(Token::minus_minus);
			++cur;
		} else if (peek() == '=') {
			tok.setType(Token::minus_equal);
			++cur;
		} else {
			tok.setType(Token::minus);
		}
		break;
	case '<':
		if (peek() == '<') {
			tok.setType(Token::less_less);
			++cur;
		} else if (peek() == '=') {
			tok.setType(Token::less_equal);
			++cur;
		} else {
			tok.setType(Token::less);
		}
		break;
	case '>':
		if (peek() == '>') {
			tok.setType(Token::greater_greater);
			++cur;
		} else if (peek() == '=') {
			tok.setType(Token::less_equal);
			++cur;
		} else {
			tok.setType(Token::less);
		}
		break;
	/* Illegal token */
	default:
		// TODO: handle unicode
		return lexIllegalToken(tok);
	}

	++cur;
	tok.setData(tok_start, cur);
	tok.setLocation(size_t(tok_start - buf->begin()));

	return true;
}
} // namespace daedalus
