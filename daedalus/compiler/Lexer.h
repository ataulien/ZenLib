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
#ifndef Daedalus_Lexer
#define Daedalus_Lexer
#include <daedalus/compiler/Token.h>
#include <daedalus/compiler/SourceBuffer.h>
#include <daedalus/compiler/KeywordMap.h>
namespace Daedalus {
/*!
 * Produces token stream from a source buffer.
 */
struct Lexer {
	Lexer(SourceBuffer* inputBuffer);

	Token currentToken();

	/*!
	 * Extract next token from stream
	 */
	Token nextToken();

private:
	void init();
	char peek() const;
	char prev() const;

	bool lexIllegalToken(Token& token);
	bool lexIdentifier(Token& token);
	bool lexStringLiteral(Token& token);
	bool lexNumericConstant(Token& token);
	bool lexNextToken(Token& token);

	void skipBlockComment();
	void handleComment();

	bool lexCommentToken(Token& tok);

	KeywordMap kwmap;
	SourceBuffer* buf;

	char const* cur;
	char const* end;

	Token cur_token;
};
} // namespace daedalus
#endif//Daedalus_Lexer
