#include "const.h"
#include "ident.h"
#include "tok.h"

namespace Const {

	bool is_declaration_start(Tokenizer &tok) {
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

	static std::unique_ptr<Value> perform_int_op(std::unique_ptr<Int_Value> &&first, Token_Type op, std::unique_ptr<Int_Value> &&second) {
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

	static std::unique_ptr<Value> perform_op(std::unique_ptr<Value> &&first, Token_Type op, std::unique_ptr<Value> &&second) {
		auto first_int { dynamic_cast<Int_Value *>(first.get()) };
		auto second_int { dynamic_cast<Int_Value *>(second.get()) };
		if (first_int && second_int) {
			return perform_int_op(
				unique_cast<Int_Value>(std::move(first)), op,
				unique_cast<Int_Value>(std::move(second))
			);
		}
		err("const op", "arguments are of wrong type");
		return nullptr;
	}

	static std::unique_ptr<Value> clone_value(Value *v) {
		if (auto i { dynamic_cast<Int_Value *>(v) }) {
			return std::make_unique<Int_Value>(i->value());
		}
		err("clone_value", "wrong value type");
		return nullptr;
	}

	static std::unique_ptr<Value> read_factor(const Mapping &mapping, Tokenizer &tok) {
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
		if (tok.type() == Token_Type::true_kw) {
			tok.next();
			return std::make_unique<Bool_Value>(true);
		}
		if (tok.type() == Token_Type::false_kw) {
			tok.next();
			return std::make_unique<Bool_Value>(false);
		}
		if (tok.type() == Token_Type::identifier) {
			auto got { mapping.get(tok.ident(), false) };
			if (got) {
				tok.next();
				return clone_value(got);
			} else { err("const_factor", "identifier not constant"); }
		}
		err("const_factor", "unknown token");
		return nullptr;
	}

	static std::unique_ptr<Value> read_term(const Mapping &mapping, Tokenizer &tok) {
		auto cur { read_factor(mapping, tok) };
		// TODO
		return cur;
	}

	static bool is_numeric(Value *v) {
		return v && dynamic_cast<Numeric_Value *>(v);
	}

	static std::unique_ptr<Value> read_simple_expression(const Mapping &mapping, Tokenizer &tok) {
		bool positive { false };
		bool negative { false };

		if (tok.type() == Token_Type::plus) {
			positive = true;
			tok.next();
		} else if (tok.type() == Token_Type::minus) {
			negative = true;
			tok.next();
		}

		auto cur { read_term(mapping, tok) };
		while (tok.type() == Token_Type::plus || tok.type() == Token_Type::minus) { // TODO
			auto op { tok.type() };
			tok.next();
			auto nxt { read_term(mapping, tok) };
			cur = perform_op(std::move(cur), op, std::move(nxt));
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

	static std::unique_ptr<Value> read_expression(const Mapping &mapping, Tokenizer &tok) {
		auto first { read_simple_expression(mapping, tok) };
		return first;
		// TODO relational
	}

	void read_declaration(Mapping &mapping, Tokenizer &tok) {
		auto id { read_ident_def(tok) };
		auto got { mapping.get(id.ident, false) };
		if (got) {
			err("const", "redeclaration of '" + id.ident + "'");
			return;
		}
		assert_next_tok(Token_Type::equals, tok, "read_const_declaration", "'=' expected");

		auto value { read_expression(mapping, tok) };
		mapping.add(id.ident, std::move(value), id.exported);
	}

	void Mapping::add(const std::string &name, std::unique_ptr<Value> value, bool exported) {
		values_[name] = std::move(value);
		if (exported) { exports_.insert(name); }
	}

	Value *Mapping::get(const std::string &name, bool exported_only) const {
		auto got { values_.find(name) };
		if (got == values_.end() || (exported_only && exports_.find(name) == exports_.end())) {
			return nullptr;
		}
		return got->second.get();
	}
}
