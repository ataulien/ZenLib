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
#ifndef Daedalus_Lexer_KeywordMap
#define Daedalus_Lexer_KeywordMap
#include <map>
#include <string>
#include <daedalus/compiler/Token.h>
namespace Daedalus {
/*!
 * Helper class for keyword lookup
 */
class KeywordMap {
public:
	/*!
	 * Add a keyword to the map.
	*/
	KeywordMap& add(std::string def, Token::Kind kind);
	/*!
	 * Get token type from string.
	 */ 
	Token::Kind get(std::string def);
	/*!
	 * Check if a string is keyword in the map.
	 */
	bool isKeyword(std::string def);
private:
	std::map<std::string, Token::Kind> theMap;
};
} // namespace daedalus
#endif//Daedalus_Lexer_KeywordMap
