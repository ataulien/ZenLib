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
#ifndef Daedaalus_OU_PP
#define Daedaalus_OU_PP
#include <set>
#include <string>
#include <daedalus/compiler/SourceBuffer.h>
#include <daedalus/compiler/Lexer.h>
namespace daedalus {
namespace ou {
struct OutputUnit {
	std::string name;
	std::string soundFile;
	std::string subtitle;
};

bool operator ==(OutputUnit const& a, OutputUnit const& b)
{
	return a.name == b.name;
}

bool operator <(OutputUnit const& a, OutputUnit const& b)
{
	return a.name < b.name;
}

// TODO: use flat_set (sorted vector) or similar?
using OutputUnitList = std::set<OutputUnit>;

/*!
 * Extracts information needed to generate OU.bin from
 * `AI_Output(self, other, "WAVE"); // Subtitle`
 * lines and C_SVM instances.
 */
struct Parser {
	Parser(SourceBuffer& buf)
		: buf(buf)
	{
		cur = buf.begin();
		end = buf.end();
	}

	OutputUnitList loadOutputUnits();
private:
	template <class UnaryPredicate>
	bool advance_if(UnaryPredicate predicate);
	bool advance(char c);

	std::string readWord();
	std::string readString();

	void processSubtitle(std::string name, OutputUnitList& list);
	void processAI_Output(OutputUnitList& units);
	void processSVMVar(OutputUnitList& units);
	void processSVMInstance(OutputUnitList& units);

	SourceBuffer& buf;
	char const* cur;
	char const* end;
};
} // namespace ou
} // namespace daedalus
#endif//Daedaalus_OU_PP
