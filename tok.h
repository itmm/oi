#pragma once

#include <iostream>
#include <map>

#include "err.h"

enum class Token_Type { end_of_file, identifier, module, end, semicolon, period, comma, colon, becomes, import };

class Tokenizer {
		std::istream &in_;
		int ch_;
		std::string ident_;

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

		Token_Type next() {
			while (ch_ != EOF && ch_ <= ' ') { get(); }
			if (isalpha(ch_)) {
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
