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
#include <cstring>
#include <daedalus/compiler/KeywordMap.h>
namespace Daedalus {
constexpr bool Case_Insensitive_Keywords = true;
/*!
 * Transform the string to lower case.
 */
static std::string& tolower(std::string& str)
{
	auto lower_char = [] (char c) { return std::tolower(c); };
	std::transform(begin(str), end(str), begin(str), lower_char);
	return str;
}

// Link token to a keyword
KeywordMap& KeywordMap::add(std::string def, Token::Kind kind)
{
	theMap[def] = kind;
	return *this;
}

// Get token type from string
Token::Kind KeywordMap::get(std::string def)
{
	if (Case_Insensitive_Keywords)
		tolower(def);

	auto result = theMap.find(def);

	if (result == theMap.end())
		// TODO: better indication that it is not present
		return Token::illegal;

	return theMap[def];
}

// Check if keyword is present
bool KeywordMap::isKeyword(std::string def)
{
	return get(def) != Token::illegal;
}
} // namespace daedalus
