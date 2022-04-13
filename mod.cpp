#include "mod.h"

void read_module(Tokenizer &tok) {
	assert_next_tok(Token_Type::module, tok, "read_module", "MODULE expected");
	assert_tok(Token_Type::identifier, tok, "read_module", "MODULE name expected");
	auto module_name { tok.ident() };
	tok.next();
	assert_next_tok(Token_Type::semicolon, tok, "read_module", "no ';' after module name");

	assert_next_tok(Token_Type::end, tok, "read_module", "END expected");
	assert_tok(Token_Type::identifier, tok, "read_module", "MODULE name after END expected");
	if (tok.ident() != module_name) { err("read_module", "MODULE name does not match END name"); }
	tok.next();
	assert_next_tok(Token_Type::period, tok, "read_module", "no '.' after END clause");
}
