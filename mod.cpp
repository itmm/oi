#include "mod.h"

static void read_import(Tokenizer &tok) {
	assert_tok(Token_Type::identifier, tok, "read_import", "module name expected");
	auto assigned_name { tok.ident() };
	std::string module_name;
	if (tok.next() == Token_Type::becomes) {
		tok.next();
		assert_tok(Token_Type::identifier, tok, "read_import", "original module name expected");
		module_name = tok.ident();
		tok.next();
	} else {
		module_name = assigned_name;
	}
}

static void read_import_list(Tokenizer &tok) {
	if (tok.type() != Token_Type::import) { return; }
	tok.next();
	read_import(tok);
	while (tok.type() == Token_Type::comma) {
		tok.next();
		read_import(tok);
	}
	assert_next_tok(Token_Type::semicolon, tok, "read_import_list", "';' expected");
}

void read_module(Tokenizer &tok) {
	assert_next_tok(Token_Type::module, tok, "read_module", "MODULE expected");
	assert_tok(Token_Type::identifier, tok, "read_module", "MODULE name expected");
	auto module_name { tok.ident() };
	tok.next();
	assert_next_tok(Token_Type::semicolon, tok, "read_module", "no ';' after module name");
	read_import_list(tok);

	assert_next_tok(Token_Type::end, tok, "read_module", "END expected");
	assert_tok(Token_Type::identifier, tok, "read_module", "MODULE name after END expected");
	if (tok.ident() != module_name) { err("read_module", "MODULE name does not match END name"); }
	tok.next();
	assert_next_tok(Token_Type::period, tok, "read_module", "no '.' after END clause");
}
