#include "exp.h"

#include "err.h"

namespace Expression {

	static std::shared_ptr<Expression> read_factor(const Mapping &mapping, Tokenizer &tok) {
		if (tok.type() == Token_Type::integer) {
			auto result { std::make_shared<Integer>(tok.integer()) };
			tok.next();
			return result;
		}
		if (tok.type() == Token_Type::real) {
			auto result { std::make_shared<Real>(tok.real()) };
			tok.next();
			return result;
		}

		// TODO: string
		// TODO: NIL
		// TODO: set
		// TODO: ~
	
		if (tok.type() == Token_Type::true_kw) {
			tok.next();
			return std::make_shared<Bool>(true);
		}
		if (tok.type() == Token_Type::false_kw) {
			tok.next();
			return std::make_shared<Bool>(false);
		}
		if (tok.type() == Token_Type::lparen) {
			tok.next();
			auto got { Expression::read(mapping, tok) };
			if (tok.type() != Token_Type::rparen) {
				err("factor", "')' expected");
			}
			return got;
		}

		if (tok.type() == Token_Type::identifier) {
			auto got { std::make_shared<Variable>(tok.ident()) };
			tok.next();
			return got;
			// TODO: function
		}
		err("factor", "unknown token");
		return nullptr;
	}

	static std::shared_ptr<Expression> read_term(const Mapping &mapping, Tokenizer &tok) {
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
			cur = std::make_shared<Binary>(cur, op, nxt);
		}
		return cur;
	}

	static std::shared_ptr<Expression> read_simple_expression(const Mapping &mapping, Tokenizer &tok) {
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
		if (positive) {
			cur = std::make_shared<Unary>(Token_Type::plus, cur);
		} else if (negative) {
			cur = std::make_shared<Unary>(Token_Type::minus, cur);
		}

		while (tok.type() == Token_Type::plus || tok.type() == Token_Type::minus || tok.type() == Token_Type::or_kw) {
			auto op { tok.type() };
			tok.next();
			auto nxt { read_term(mapping, tok) };
			cur = std::make_shared<Binary>(cur, op, nxt);
		}
		return cur;

	}

	std::shared_ptr<Expression> Expression::read(const Mapping &mapping, Tokenizer &tok) {
		auto first { read_simple_expression(mapping, tok) };
		return first;
		// TODO relational
	}
}
