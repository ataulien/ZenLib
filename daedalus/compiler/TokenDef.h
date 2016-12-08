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
TOKEN(illegal)
TOKEN(eof)
TOKEN(identifier)
TOKEN(numeric_constant)
TOKEN(string_literal)
TOKEN(block_comment)
TOKEN(line_comment)

PUNCT(comma,            ",")
PUNCT(dot,              ".")
PUNCT(semicolon,        ";")
PUNCT(l_brace,          "{")
PUNCT(r_brace,          "}")
PUNCT(l_bracket,        "[")
PUNCT(r_bracket,        "]")
PUNCT(l_paren,          "(")
PUNCT(r_paren,          ")")
PUNCT(amp,              "&")
PUNCT(amp_amp,          "&&")
PUNCT(pipe,             "|")
PUNCT(pipe_pipe,        "||")
PUNCT(ast,              "*")
PUNCT(ast_equal,        "*=")
PUNCT(plus,	        "+")
PUNCT(plus_equal,       "+=")
PUNCT(plus_plus,        "++")
PUNCT(minus,	        "-")
PUNCT(minus_equal,      "-=")
PUNCT(minus_minus,      "--")
PUNCT(slash,	        "/")
PUNCT(slash_equal,      "/=")
PUNCT(percent,	        "%")
PUNCT(less,	        "<")
PUNCT(less_equal,       "<=")
PUNCT(less_less,        "<<")
PUNCT(greater,          ">")
PUNCT(greater_equal,    ">=")
PUNCT(greater_greater,  ">>")
PUNCT(bang,             "!")
PUNCT(bang_equal,       "!=")
PUNCT(tilde,            "~")
PUNCT(caret,            "^")
PUNCT(equal,            "=")
PUNCT(equal_equal,      "==")

TOKEN(keyword)
KEYWORD(const)
KEYWORD(var)
KEYWORD(func)
KEYWORD(if)
KEYWORD(else)
KEYWORD(class)
KEYWORD(prototype)
KEYWORD(instance)
KEYWORD(return)
KEYWORD(void)
KEYWORD(float)
KEYWORD(int)
KEYWORD(string)

// extensions
KEYWORD(extern)
KEYWORD(while)
KEYWORD(do)
KEYWORD(break)
KEYWORD(continue)
