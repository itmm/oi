#include "tok.h"

std::map<std::string, Token_Type> Tokenizer::keywords_ {
	{ "MODULE"s, Token_Type::module },
	{ "END"s, Token_Type::end },
	{ "IMPORT"s, Token_Type::import }
};
