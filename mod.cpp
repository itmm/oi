#include "mod.h"

#include <map>
#include <memory>

class Value {
	public:
		virtual ~Value() { }
};

class Module {
		const std::string name_;
		std::map<std::string, Module *> imports_;
		static std::map<std::string, Module *> all_modules_;
		std::map<std::string, std::unique_ptr<Value>> constants_;

	public:
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
		Value *get_const(const std::string &name) {
			auto got { constants_.find(name) };
			return got != constants_.end() ? got->second.get() : nullptr;
		}
		void add_const(const std::string &name, std::unique_ptr<Value> value, bool exported) {
			if (! get_const(name)) {
				constants_[name] = std::move(value);
			} else { err("Module", "redefinition of constant"); }
		}
};

std::map<std::string, Module *> Module::all_modules_;

#include <fstream>

static bool is_const_declaration_start(Tokenizer &tok) {
	switch (tok.type()) {
		case Token_Type::type_kw:
		case Token_Type::var_kw:
		case Token_Type::procedure_kw:
		case Token_Type::begin_kw:
		case Token_Type::end_kw:
			return false;
		default:
			return true;
	}
}

struct Ident_Def {
	const std::string ident;
	bool exported;
	Ident_Def(const std::string &i, bool e) : ident { i }, exported { e } { }
};

static Ident_Def read_ident_def(Tokenizer &tok) {
	assert_tok(Token_Type::identifier, tok, "read_ident_def", "name expected");
	auto name { tok.ident() };
	tok.next();
	bool exported { tok.type() == Token_Type::asterisk };
	if (exported) { tok.next(); }
	return { name, exported };
}


class Numeric_Value: public Value { };

class Int_Value: public Numeric_Value {
		int value_;
	public:
		Int_Value(int v): value_ { v } { }
		int value() const { return value_; }
};

class Real_Value: public Numeric_Value {
		double value_;
	public:
		Real_Value(double v): value_ { v } { }
		double value() const { return value_; }
};

std::unique_ptr<Value> perform_const_int_op(std::unique_ptr<Int_Value> &&first, Token_Type op, std::unique_ptr<Int_Value> &&second) {
	err("const_int_op", "not implemented yet");
	return nullptr;
}

template<typename TO, typename FROM>
inline std::unique_ptr<TO> unique_cast(std::unique_ptr<FROM> from) {
	auto tmp { dynamic_cast<TO *>(from.get()) };
	if (tmp) {
		std::unique_ptr<TO> result;
		from.release();
		result.reset(tmp);
		return result;
	} else { return nullptr; }
}

std::unique_ptr<Value> perform_const_op(std::unique_ptr<Value> &&first, Token_Type op, std::unique_ptr<Value> &&second) {
	auto first_int { dynamic_cast<Int_Value *>(first.get()) };
	auto second_int { dynamic_cast<Int_Value *>(second.get()) };
	if (first_int && second_int) {
	       	return perform_const_int_op(
			unique_cast<Int_Value>(std::move(first)), op,
		       	unique_cast<Int_Value>(std::move(second))
		);
       	}
	err("const op", "arguments are of wrong type");
	return nullptr;
}

std::unique_ptr<Value> clone_value(Value *v) {
	if (auto i { dynamic_cast<Int_Value *>(v) }) {
		return std::make_unique<Int_Value>(i->value());
	}
	err("clone_value", "wrong value type");
	return nullptr;
}

std::unique_ptr<Value> read_const_factor(Module *mod, Tokenizer &tok) {
	if (tok.type() == Token_Type::integer) {
		auto result { std::make_unique<Int_Value>(tok.integer()) };
		tok.next();
		return result;
	}
	if (tok.type() == Token_Type::real) {
		auto result { std::make_unique<Real_Value>(tok.real()) };
		tok.next();
		return result;
	}
	if (tok.type() == Token_Type::identifier) {
		auto got { mod->get_const(tok.ident()) };
		if (got) {
			tok.next();
			return clone_value(got);
		} else { err("const_factor", "identifier not constant"); }
	}
	err("const_factor", "unknown token");
	return nullptr;
}

std::unique_ptr<Value> read_const_term(Module *mod, Tokenizer &tok) {
	auto cur { read_const_factor(mod, tok) };
	// TODO
	return cur;
}

bool is_numeric(Value *v) {
	return v && dynamic_cast<Numeric_Value *>(v);
}

std::unique_ptr<Value> read_simple_const_expression(Module *mod, Tokenizer &tok) {
	bool positive { false };
	bool negative { false };

	if (tok.type() == Token_Type::plus) {
		positive = true;
		tok.next();
	} else if (tok.type() == Token_Type::minus) {
		negative = true;
		tok.next();
	}

	auto cur { read_const_term(mod, tok) };
	while (tok.type() == Token_Type::plus || tok.type() == Token_Type::minus) { // TODO
		auto op { tok.type() };
		tok.next();
		auto nxt { read_const_term(mod, tok) };
		cur = perform_const_op(std::move(cur), op, std::move(nxt));
	}
	if (positive) {
		if (! is_numeric(cur.get())) {
			err("read_const", "'+' without numeric");
			return nullptr;
		}
		return cur;
	}
	if (negative) {
		if (! is_numeric(cur.get())) {
			err("read_const", "'-' without numeric");
			return nullptr;
		}
		if (auto c { dynamic_cast<Int_Value *>(cur.get()) }) {
			return std::make_unique<Int_Value>(-c->value());
		}
		err("read_const", "unknown numeric type");
		return nullptr;
	}
	return cur;

}

std::unique_ptr<Value> read_const_expression(Module *mod, Tokenizer &tok) {
	auto first { read_simple_const_expression(mod, tok) };
	return first;
	// TODO relational
}

static void read_const_declaration(Module *mod, Tokenizer &tok) {
	auto id { read_ident_def(tok) };
	assert_next_tok(Token_Type::equals, tok, "reod_const_declaration", "'=' expected");

	auto value { read_const_expression(mod, tok) };
	mod->add_const(id.ident, std::move(value), id.exported);
}

static void read_declaration_sequence(Module *mod, Tokenizer &tok) {
	if (tok.type() == Token_Type::const_kw) {
		tok.next();
		while (is_const_declaration_start(tok)) {
			read_const_declaration(mod, tok);
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
