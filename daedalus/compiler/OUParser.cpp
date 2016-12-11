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
#include <cassert>
#include <algorithm>
#include <daedalus/StringUtils.h>
#include <daedalus/compiler/OUParser.h>
namespace daedalus {
namespace ou {
namespace {
char const* skipComment(char const* cur, char const* end)
{
	++cur;
	if (*cur == '/')
		return std::find(cur, end, '\n');

	if (*cur != '*')
		return cur;

	cur += 2;
	while (true) {
		cur = std::find(cur, end, '/');

		if (cur == end)
			return cur;

		char const* prev = cur++ - 1;
		if (*prev == '*')
			return cur;
	}
}

char const* skipJunk(char const* cur, char const* end)
{
	if (isspace(*cur))
		return std::find_if_not(cur, end, isspace);

	if (cur == end)
		return cur;

	if (*cur == '/')
		return skipComment(cur, end);

	return cur + 1;
}
} // namespace

template <class Predicate>
bool Parser::advance_if(Predicate predicate)
{
	for (; cur != end; cur = skipJunk(cur, end)) {
		if (predicate(*cur))
			return true;
	}
	return false;
}

bool Parser::advance(char c)
{
	if (advance_if([c] (char x) {return x == c;})) {
		++cur;
		return true;
	}
	return false;
}

std::string Parser::readWord()
{
	auto* start = cur++;

	advance_if([] (char x) {return !isalnum(x) && x != '_';});

	return std::string(start, cur);
}

std::string Parser::readString()
{
	std::string str;
	for (; cur != end; ++cur) {
		if (*cur == '"')
			break;
		if (*cur == '\\')
			++cur;
		str += *cur;
	}
	++ cur; // skip '"'
	return str;
}

OutputUnitList Parser::loadOutputUnits()
{
	OutputUnitList ou;

	while (*cur) {
		advance_if(isalpha);
		std::string id = readWord();
		string::tolower(id);

		if (id == "ai_output") {
			processAI_Output(ou);
		} else if (id == "instance") {
			processSVMInstance(ou);
		}
	}

	return ou;
}

void Parser::processSubtitle(std::string name, OutputUnitList& list)
{
	cur = std::find_if_not(cur, end, isspace);

	if (*cur++ != '/') return;
	if (*cur++ != '/') return;

	auto start = std::find_if_not(cur, end, isspace);
	cur = std::find(start, end, '\n');

	auto comment = std::string(start, cur);

	OutputUnit ou;
	ou.name = name;
	ou.subtitle = comment;
	ou.soundFile = name + ".wav";
	list.insert(ou);
}

void Parser::processAI_Output(OutputUnitList& list)
{
	advance('(');
	advance(',');
	advance(',');

	if (!advance('"')) return;

	auto name = readString();

	advance(')');
	advance(';');

	processSubtitle(name, list);
}

void Parser::processSVMVar(OutputUnitList& list)
{
	if (!advance_if(isalpha)) return;
	if (!advance('=')) return;
	if (!advance('"')) return;

	auto name = readString();

	advance(';');
	processSubtitle(name, list);
}

void Parser::processSVMInstance(OutputUnitList& list)
{
	if (!advance_if(isalpha)) return;
	advance('(');
	advance_if(isalpha);

	if (readWord() != "C_SVM") {
		// Hopefully, nobody puts AI_Output inside instances
		advance('}');
		return;
	}

	advance(')');

	if (!advance('{')) return;

	while (*cur) {
		processSVMVar(list);
		cur = skipJunk(cur, end);
		if (*cur == '}')
			break;
	};
}
} // namespace ou
} // namespace daedalus
