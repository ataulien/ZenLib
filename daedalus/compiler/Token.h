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
#ifndef Daedalus_Lexer_Token
#define Daedalus_Lexer_Token
#include <cassert>
#include <string>
namespace Daedalus {

struct Location {
	Location() = default;
	Location(size_t offset)
		: pos(offset)
	{
		assert(offset < (1u << 31));
	}
	std::uint32_t fileId;
	std::uint32_t pos;
};

struct Token {
	/*!
	 * Enumeration of all token kinds,
	 * All kinds are defined if TokenDef.h
	 */
	enum Kind {
#define TOKEN(x) x,
#define PUNCT(x, y) TOKEN(x)
#define KEYWORD(x) kw_ ## x,
#include <daedalus/compiler/TokenDef.h>
#undef TOKEN
#undef PUNCT
#undef KEYWORD
	};

	// TODO: Illegal
	Token() : kind(Token::illegal) { }
	Token(char const* start, char const* end)
		: start(start), end(end)
	{
	}

	Kind type() const
	{
		return kind;
	}
	
	void setType(Kind newKind)
	{
		kind = newKind;
	}

	std::string data() const
	{
		return std::string(start, end);
	}

	void setData(char const* start, char const* end)
	{
		this->start = start;
		this->end = end;
	}

	void setLocation(Location newLoc)
	{
		loc = newLoc;
	}

	Location location()
	{
		return loc;
	}

private:
	Kind kind;
	char const* start;
	char const* end;
	Location loc;
};

inline bool isIdentifier(Token tok)
{
	// TODO: Identifier
	return tok.type() == Token::identifier;
}

inline bool operator == (Token tok, Token::Kind kind)
{
	return tok.type() == kind;
}
} // namespace daedalus
#endif//Daedalus_Lexer_Token
