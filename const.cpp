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

	static int perform_int_op(int first, Token_Type op, int second) {
		switch (op) {
			case Token_Type::plus: return first + second;
			case Token_Type::minus: return first - second;
			case Token_Type::asterisk: return first * second;
			case Token_Type::div_kw: return first / second;
			case Token_Type::mod_kw: return first % second;
			default:
				err("const_int_op", "not implemented yet");
				return -1;
		}
	}

	static double perform_real_op(double first, Token_Type op, double second) {
		switch (op) {
			case Token_Type::plus: return first + second;
			case Token_Type::minus: return first - second;
			case Token_Type::asterisk: return first * second;
			case Token_Type::div: return first / second;
			default:
				err("const_real_op", "not implemented yet");
				return -1.0;
		}
	}

	static bool perform_bool_op(bool first, Token_Type op, bool second) {
		switch (op) {
			case Token_Type::or_kw: return first | second;
			case Token_Type::and_sym: return first & second;
			default:
				err("const_bool_op", "not implemented yet");
				return false;
		}
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
			return std::make_unique<Int_Value>(perform_int_op(first_int->value(), op, second_int->value()));
		}
		auto first_real { dynamic_cast<Real_Value *>(first.get()) };
		auto second_real { dynamic_cast<Real_Value *>(second.get()) };
		if (first_real || second_real) {
			double a, b;
			if (! first_real) {
				if (first_int) { a = first_int->value(); }
				else err("const op", "first number is not real");
			} else { a = first_real->value(); }
			if (! second_real) {
				if (second_int) { b = second_int->value(); }
				else err("const op", "second number is not real");
			} else { b = first_real->value(); }
			return std::make_unique<Real_Value>(perform_real_op(a, op, b));
		}
		auto first_bool { dynamic_cast<Bool_Value *>(first.get()) };
		auto second_bool { dynamic_cast<Bool_Value *>(second.get()) };
		if (first_bool && second_bool) {
			return std::make_unique<Bool_Value>(perform_bool_op(first_bool->value(), op, second_bool->value()));
		}
		err("const op", "arguments are of wrong type");
		return nullptr;
	}

	static std::unique_ptr<Value> clone_value(Value *v) {
		if (auto i { dynamic_cast<Int_Value *>(v) }) {
			return std::make_unique<Int_Value>(i->value());
		}
		if (auto r { dynamic_cast<Real_Value *>(v) }) {
			return std::make_unique<Real_Value>(r->value());
		}
		if (auto b { dynamic_cast<Bool_Value *>(v) }) {
			return std::make_unique<Bool_Value>(b->value());
		}
		if (auto s { dynamic_cast<String_Value *>(v) }) {
			return std::make_unique<String_Value>(s->value());
		}
		err("clone_value", "wrong value type");
		return nullptr;
	}

	static std::unique_ptr<Value> read_expression(const Mapping &mapping, Tokenizer &tok);

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

		// TODO: char constant
		// TODO: string
		// TODO: NIL
		// TODO: set
		// TODO: ~
	
		if (tok.type() == Token_Type::true_kw) {
			tok.next();
			return std::make_unique<Bool_Value>(true);
		}
		if (tok.type() == Token_Type::false_kw) {
			tok.next();
			return std::make_unique<Bool_Value>(false);
		}
		if (tok.type() == Token_Type::lparen) {
			tok.next();
			auto got { read_expression(mapping, tok) };
			if (tok.type() != Token_Type::rparen) {
				err("const_factor", "')' expected");
			}
			return got;
		}

		if (tok.type() == Token_Type::identifier) {
			auto got { mapping.get(tok.ident(), false) };
			if (got) {
				tok.next();
				return clone_value(got);
			} else { err("const_factor", "identifier not constant"); }
			// TODO: function
		}
		err("const_factor", "unknown token");
		return nullptr;
	}

	static std::unique_ptr<Value> read_term(const Mapping &mapping, Tokenizer &tok) {
		auto cur { read_factor(mapping, tok) };

		while (tok.type() == Token_Type::asterisk ||
			tok.type() == Token_Type::div ||
			tok.type() == Token_Type::div_kw || 
			tok.type() == Token_Type::mod_kw || 
			tok.type() == Token_Type::and_sym
		) {
			auto op { tok.type() };
			tok.next();
			auto nxt { read_factor(mapping, tok) };
			cur = perform_op(std::move(cur), op, std::move(nxt));
		}
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
		while (tok.type() == Token_Type::plus || tok.type() == Token_Type::minus || tok.type() == Token_Type::or_kw) {
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
