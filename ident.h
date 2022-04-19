#pragma once

#include <string>

#include "mapping.h"
#include "mod.h"
#include "tok.h"

struct Ident_Def {
	std::string ident;
	bool exported;

	static Ident_Def read(Tokenizer &tok);
};

struct Qual_Ident {
	std::shared_ptr<Module> mod;
	std::shared_ptr<Obj> obj;

	static Qual_Ident read(const Mapping &mapping, Tokenizer &tok);
};
