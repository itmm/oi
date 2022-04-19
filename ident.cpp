#include "ident.h"

Ident_Def Ident_Def::read(Tokenizer &tok) {
	Ident_Def result;

	assert_tok(Token_Type::identifier, tok, "read_ident_def", "name expected");
	result.ident  = tok.ident();
	tok.next();
	result.exported = tok.type() == Token_Type::asterisk;
	if (result.exported) { tok.next(); }

	return result;
}

Qual_Ident Qual_Ident::read(const Mapping &mapping, Tokenizer &tok) {
	Qual_Ident result;

	assert_tok(Token_Type::identifier, tok, "qual_ident_def", "name expected");
	std::string name = tok.ident();
	tok.next();
	result.obj = mapping.get(name, false);
	auto mod { dynamic_cast<Module *>(result.obj.get()) };
	if (mod) {
		result.mod = std::dynamic_pointer_cast<Module>(result.obj);
		assert_next_tok(Token_Type::period, tok, "qual_ident_def", "'.' after module expected");
		assert_tok(Token_Type::identifier, tok, "qual_ident_def", "identfier after module expected");
		name = tok.ident();
		tok.next();
		result.obj = result.mod->mapping.get(name, true);
	}
	if (! result.obj) {
		err("qual_ident_def", "object not found");
	}

	return result;
}

