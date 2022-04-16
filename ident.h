#pragma once

#include <string>

#include "tok.h"

struct Ident_Def {
	const std::string ident;
	bool exported;
	Ident_Def(const std::string &i, bool e) : ident { i }, exported { e } { }
};

Ident_Def read_ident_def(Tokenizer &tok);
