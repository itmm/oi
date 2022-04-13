#pragma once

#include <iostream>

#include "err.h"

enum class Token_Type { end_of_file, identifier };

class Tokenizer {
		std::istream &in_;
		int ch_;
		Token_Type type_ { Token_Type::end_of_file };


		int get() {
			if (ch_ != EOF) { ch_ = in_.get(); }
			return ch_;
		}

		Token_Type set_token(const Token_Type &type) {
			type_ = type;
			return type_;
		}

	public:
		Tokenizer(std::istream &in): in_ { in }, ch_ { ' ' } {
			next();
		}

		Token_Type type() const { return type_; }

		Token_Type next() {
			while (ch_ != EOF && ch_ <= ' ') { get(); }
			if (isalpha(ch_)) {
				while (isalnum(ch_)) { get(); }
				return set_token(Token_Type::identifier);
			} else switch (ch_) {
				case EOF: return set_token(Token_Type::end_of_file);
				default:
					  err("token", "unknown char '"s + static_cast<char>(ch_) + "'"s);
					  throw 10;
			}
		}
};
