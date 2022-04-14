#include "tok.h"

std::map<std::string, Token_Type> Tokenizer::keywords_ {
	{ "MODULE"s, Token_Type::module_kw },
	{ "END"s, Token_Type::end_kw },
	{ "IMPORT"s, Token_Type::import_kw },
	{ "CONST"s, Token_Type::const_kw },
	{ "TYPE"s, Token_Type::type_kw },
	{ "VAR"s, Token_Type::var_kw },
	{ "PROCEDURE"s, Token_Type::procedure_kw },
	{ "BEGIN"s, Token_Type::begin_kw }
};
