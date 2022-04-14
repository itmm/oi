#pragma once

#include <iostream>
#include <map>

#include "err.h"

enum class Token_Type {
       	end_of_file, identifier, module_kw, end_kw, semicolon, period, comma,
       	colon, becomes, import_kw, const_kw, type_kw, procedure_kw, var_kw,
       	begin_kw, asterisk, equals, plus, minus, integer
};

class Tokenizer {
		std::istream &in_;
		int ch_;
		std::string ident_;
		int integer_;

		Token_Type type_ { Token_Type::end_of_file };

		int get() {
			if (ch_ != EOF) { ch_ = in_.get(); }
			return ch_;
		}

		Token_Type set_token(const Token_Type &type) {
			type_ = type;
			return type_;
		}

		static std::map<std::string, Token_Type> keywords_;

	public:
		Tokenizer(std::istream &in): in_ { in }, ch_ { ' ' } {
			next();
		}

		Token_Type type() const { return type_; }
		std::string ident() const { return ident_; }
		int integer() const { return integer_; }

		Token_Type next() {
			while (ch_ != EOF && ch_ <= ' ') { get(); }
			if (isdigit(ch_)) {
				integer_ = 0;
				while (isdigit(ch_)) {
					integer_ = integer_ * 10 + (ch_ - '0');
					get();
				}
				return set_token(Token_Type::integer);
			} else if (isalpha(ch_)) {
				ident_.clear();
				while (isalnum(ch_)) { ident_ += static_cast<char>(ch_); get(); }
				auto got { keywords_.find(ident_) };
				if (got != keywords_.end()) {
					return set_token(got->second);
				}

				return set_token(Token_Type::identifier);
			} else switch (ch_) {
				case EOF: return set_token(Token_Type::end_of_file);
				case ';': get(); return set_token(Token_Type::semicolon);
				case '.': get(); return set_token(Token_Type::period);
				case ':': {
					get();
					if (ch_ == '=') {
						get(); return set_token(Token_Type::becomes);
					}
					return set_token(Token_Type::colon);
				}
				case ',': get(); return set_token(Token_Type::comma);
				case '*': get(); return set_token(Token_Type::asterisk);
				case '=': get(); return set_token(Token_Type::equals);
				case '+': get(); return set_token(Token_Type::plus);
				case '-': get(); return set_token(Token_Type::minus);
				default:
					  err("token", "unknown char '"s + static_cast<char>(ch_) + "'"s);
					  throw 10;
			}
		}
};

inline void assert_tok(Token_Type type, Tokenizer &tok, const std::string &from, const std::string &msg) {
	if (tok.type() != type) { err(from, msg); }
}

inline void assert_next_tok(Token_Type type, Tokenizer &tok, const std::string &from, const std::string &msg) {
	assert_tok(type, tok, from, msg);
	tok.next();
}
