#include "mod.h"
#include "const.h"

std::map<std::string, std::shared_ptr<Module>> Module::all_modules_;

#include <fstream>

static void read_declaration_sequence(Module &mod, Tokenizer &tok) {
	if (tok.type() == Token_Type::const_kw) {
		tok.next();
		while (Const::Value::is_start(tok)) {
			Const::Value::read(mod.mapping, tok);
			assert_next_tok(Token_Type::semicolon, tok, "read_declaration", "no ';' after const declaration");
		}
	}
}

static void read_import(Module &mod, Tokenizer &tok) {
	assert_tok(Token_Type::identifier, tok, "read_import", "module name expected");
	auto assigned_name { tok.ident() };
	if (mod.mapping.has(assigned_name)) {
		err("read_import", "double import of name '"s + assigned_name + "'"s);
	}
	std::string module_name;
	if (tok.next() == Token_Type::becomes) {
		tok.next();
		assert_tok(Token_Type::identifier, tok, "read_import", "original module name expected");
		module_name = tok.ident();
		tok.next();
	} else {
		module_name = assigned_name;
	}
	auto imp { Module::find(module_name) };
	if (! imp) {
		std::ifstream in { (module_name + ".mod").c_str() };
		Tokenizer t2 { in };
		imp = read_module(t2);
		if (imp->name() != module_name) {
			err("read_import", "module '"s + module_name + ".mod' has wrong name "s + imp->name());
		}
	}
	mod.mapping.add(assigned_name, imp, false);
}

static void read_import_list(Module &mod, Tokenizer &tok) {
	if (tok.type() != Token_Type::import_kw) { return; }
	tok.next();
	read_import(mod, tok);
	while (tok.type() == Token_Type::comma) {
		tok.next();
		read_import(mod, tok);
	}
	assert_next_tok(Token_Type::semicolon, tok, "read_import_list", "';' expected");
}

std::shared_ptr<Module> read_module(Tokenizer &tok) {
	assert_next_tok(Token_Type::module_kw, tok, "read_module", "MODULE expected");
	assert_tok(Token_Type::identifier, tok, "read_module", "MODULE name expected");
	auto mod { Module::create(tok.ident()) };
	if (! mod) { err("read_module", "can't create"); return nullptr; }
	tok.next();
	assert_next_tok(Token_Type::semicolon, tok, "read_module", "no ';' after module name");
	read_import_list(*mod, tok);
	read_declaration_sequence(*mod, tok);

	if (tok.type() == Token_Type::begin_kw) {
		tok.next();
		mod->init = Statement::List::read(mod->mapping, tok);
		if (! mod->init) { err("read_module", "no init"); }
		mod->init->eval(mod->mapping);
	}

	assert_next_tok(Token_Type::end_kw, tok, "read_module", "END expected");
	assert_tok(Token_Type::identifier, tok, "read_module", "MODULE name after END expected");
	if (tok.ident() != mod->name()) { err("read_module", "MODULE name does not match END name"); }
	tok.next();
	assert_next_tok(Token_Type::period, tok, "read_module", "no '.' after END clause");
	return mod;
}
