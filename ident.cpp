#include "ident.h"

Ident_Def read_ident_def(Tokenizer &tok) {
	assert_tok(Token_Type::identifier, tok, "read_ident_def", "name expected");
	auto name { tok.ident() };
	tok.next();
	bool exported { tok.type() == Token_Type::asterisk };
	if (exported) { tok.next(); }
	return { name, exported };
}

