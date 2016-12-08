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
#ifndef Daedalus_PrintToken
#define Daedalus_PrintToken
#include <string>
#include <daedalus/compiler/Token.h>
namespace Daedalus {
/*!
 * Print token name
 */
inline std::string printToken(Token::Kind type);

/*!
 * Print sequence of characters which would produce the token
 */
inline std::string spellToken(Token::Kind kind);

/*!
 * Get characters compirising the token.
 */
inline std::string spellToken(Token token)
{
	return token.data();
}


// some macro magic to print token names
#define STR1(x) #x
#define STR(x)  STR1(x)
#define TOKEN_NAME(x) Token::x
#define KEYWORD_NAME(x) Token::kw_ ## x

inline std::string nameToken(Token::Kind type)
{
	switch (type) {
#define TOKEN(x)   case TOKEN_NAME(x)   : return STR(TOKEN1(x));
#define PUNCT(x, y) TOKEN(x)
#define KEYWORD(x) case KEYWORD_NAME(x) : return STR(KEYWORD1(x));
#include <daedalus/compiler/TokenDef.h>
#undef TOKEN
#undef PUNCT
#undef KEYWORD
	}
}

inline std::string spellToken(Token::Kind kind)
{
	switch (kind) {
#define TOKEN(x)    case TOKEN_NAME(x)   : return STR(x);
#define PUNCT(x, y) case TOKEN_NAME(x)   : return y;
#define KEYWORD(x)  case KEYWORD_NAME(x) : return STR(x);
#include <daedalus/compiler/TokenDef.h>
#undef TOKEN
#undef PUNCT
#undef KEYWORD
	}

	return "";
}

#undef TOKEN_NAME
#undef KEYWORD_NAME
#undef STR
#undef STR1
} // namespace daedalus
#endif//Daedalus_PrintToken
