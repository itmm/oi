#pragma once

#include <iostream>
#include <map>

#include "err.h"

enum class Token_Type {
       	end_of_file, identifier, module_kw, end_kw, semicolon, period, comma,
       	colon, becomes, import_kw, const_kw, type_kw, procedure_kw, var_kw,
       	begin_kw, asterisk, equals, plus, minus, integer, real, true_kw,
	false_kw, nil_kw, or_kw, div, and_sym, div_kw, mod_kw, lparen,
	rparen, if_kw, then_kw, elsif_kw, else_kw, do_kw, while_kw
};

class Tokenizer {
		std::istream &in_;
		int ch_;
		std::string ident_;
		int integer_;
		double real_;

		Token_Type type_ { Token_Type::end_of_file };

		int get() {
			if (ch_ != EOF) { ch_ = in_.get(); }
			return ch_;
		}

		Token_Type set_token(const Token_Type &type) {
			type_ = type;
			return type_;
		}

		Token_Type get_and_set_token(const Token_Type &type) {
			get();
			return set_token(type);
		}

		Token_Type get_real();
		Token_Type get_integer();
		Token_Type get_number();
		Token_Type get_ident();
		void read_comment();

		static std::map<std::string, Token_Type> keywords_;

	public:
		Tokenizer(std::istream &in): in_ { in }, ch_ { ' ' } {
			next();
		}

		Token_Type type() const { return type_; }
		std::string ident() const { return ident_; }
		int integer() const { return integer_; }
		double real() const { return real_; }

		Token_Type next();
};

inline void assert_tok(Token_Type type, Tokenizer &tok, const std::string &from, const std::string &msg) {
	if (tok.type() != type) { err(from, msg); }
}

inline void assert_next_tok(Token_Type type, Tokenizer &tok, const std::string &from, const std::string &msg) {
	assert_tok(type, tok, from, msg);
	tok.next();
}
