#include "mod.h"
#include "const.h"

#include <map>
#include <memory>

class Module {
		const std::string name_;
		std::map<std::string, Module *> imports_;
		static std::map<std::string, Module *> all_modules_;

	public:
		Const::Mapping constants;

		Module(std::string name): name_ { name } {
			all_modules_.emplace(name_, this);
		}

		const std::string &name() const { return name_; }
		bool has_import(const std::string &name) {
			return imports_.find(name) != imports_.end();
		}
		void add_import(const std::string &name, Module *mod) {
			imports_[name] = mod;
		}
		static Module *find(const std::string &name) {
			auto got { all_modules_.find(name) };
			return got != all_modules_.end() ? got->second : nullptr;
		}
};

std::map<std::string, Module *> Module::all_modules_;

#include <fstream>

static void read_declaration_sequence(Module *mod, Tokenizer &tok) {
	if (tok.type() == Token_Type::const_kw) {
		tok.next();
		while (Const::is_declaration_start(tok)) {
			Const::read_declaration(mod->constants, tok);
			assert_next_tok(Token_Type::semicolon, tok, "read_declaration", "no ';' after const declaration");
		}
	}

}

static void read_import(Module *mod, Tokenizer &tok) {
	assert_tok(Token_Type::identifier, tok, "read_import", "module name expected");
	auto assigned_name { tok.ident() };
	if (mod->has_import(assigned_name)) {
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
	Module *imp { Module::find(module_name) };
	if (! imp) {
		std::ifstream in { (module_name + ".mod").c_str() };
		Tokenizer t2 { in };
		imp = read_module(t2);
		if (imp->name() != module_name) {
			err("read_import", "module '"s + module_name + ".mod' has wrong name "s + imp->name());
		}
	}
	mod->add_import(assigned_name, imp);
}

static void read_import_list(Module *mod, Tokenizer &tok) {
	if (tok.type() != Token_Type::import_kw) { return; }
	tok.next();
	read_import(mod, tok);
	while (tok.type() == Token_Type::comma) {
		tok.next();
		read_import(mod, tok);
	}
	assert_next_tok(Token_Type::semicolon, tok, "read_import_list", "';' expected");
}

Module *read_module(Tokenizer &tok) {
	assert_next_tok(Token_Type::module_kw, tok, "read_module", "MODULE expected");
	assert_tok(Token_Type::identifier, tok, "read_module", "MODULE name expected");
	auto mod { new Module { tok.ident() } };
	tok.next();
	assert_next_tok(Token_Type::semicolon, tok, "read_module", "no ';' after module name");
	read_import_list(mod, tok);
	read_declaration_sequence(mod, tok);

	assert_next_tok(Token_Type::end_kw, tok, "read_module", "END expected");
	assert_tok(Token_Type::identifier, tok, "read_module", "MODULE name after END expected");
	if (tok.ident() != mod->name()) { err("read_module", "MODULE name does not match END name"); }
	tok.next();
	assert_next_tok(Token_Type::period, tok, "read_module", "no '.' after END clause");
	return mod;
}
