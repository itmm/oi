#pragma once

#include <iostream>
#include <map>

#include "err.h"

enum class Token_Type {
       	end_of_file, identifier, module_kw, end_kw, semicolon, period, comma,
       	colon, becomes, import_kw, const_kw, type_kw, procedure_kw, var_kw,
       	begin_kw, asterisk, equals, plus, minus, integer, real
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

		static std::map<std::string, Token_Type> keywords_;

	public:
		Tokenizer(std::istream &in): in_ { in }, ch_ { ' ' } {
			next();
		}

		Token_Type type() const { return type_; }
		std::string ident() const { return ident_; }
		int integer() const { return integer_; }
		double real() const { return real_; }

		Token_Type next() {
			while (ch_ != EOF && ch_ <= ' ') { get(); }
			if (isalnum(ch_)) {
				ident_.clear();
				while (isalnum(ch_)) { ident_ += static_cast<char>(ch_); get(); }
				auto got { keywords_.find(ident_) };
				if (got != keywords_.end()) {
					return set_token(got->second);
				}
				if (isdigit(ident_[0])) {
					bool is_real { ch_ == '.' };
					if (is_real) {
						while (ch_ == '.' || isdigit(ch_) || ch_ == '+' || ch_ == '-' || ch_ == 'E') {
							ident_ += ch_; get();
						}
						real_ = 0.0;
						auto i { ident_.begin() };
						auto end { ident_.end() };
						for (; *i != '.'; ++i) {
							if (! isdigit(*i)) {
								err("next", "invalid float pre digit");
								return set_token(Token_Type::end_of_file);
							}
							real_ = real_ * 10 + *i - '0';
						}
						++i;
						double mult { .1 };
						for (; i != end && isdigit(*i); ++i) {
							real_ += mult * (*i - '0');
							mult *= .1;
						}
						if (i != end) {
							if (*i != 'E') {
								err("next", "scaling expected");
								return set_token(Token_Type::end_of_file);
							}
							++i;
							bool neg { false };
							if (i != end && *i == '+') { ++i; }
							else if (i != end && *i == '-') { ++i; neg = true; }
							if (i == end) {
								err("next", "no scaling factor");
								return set_token(Token_Type::end_of_file);
							}
							int f { 0 };
							for (; i != end; ++i) {
								if (! isdigit(*i)) {
									err("next", "invalid scaling factor");
									return set_token(Token_Type::end_of_file);
								}
								f = f * 10 + *i - '0';
							}
							double m { 1.0 };
							double x { 2.0 };
							for (; f; f /= 2, x *= 2.0) {
								if (f % 2) {
									m *= x;
								}
							}
							if (neg) { real_ /= x; } else { real_ *= x; }

						}
						return set_token(Token_Type::real);
					} else {
						integer_ = 0;
						bool is_hex { ident_.back() == 'H' };
						auto end { ident_.end() };
						if (is_hex) {
							--end;
							for (auto i { ident_.begin() }; i != end; ++i) {
								if (! isdigit(*i) && (*i < 'A' || *i > 'F')) {
									err("next", "invalid hex digit");
									return set_token(Token_Type::end_of_file);
								}
								integer_ *= 16;
								if (isdigit(*i)) { integer_ += *i - '0'; } else { integer_ += *i - 'A' + 10; }
							}
					       	} else {
							for (auto i { ident_.begin() }; i != end; ++i) {
								if (! isdigit(*i)) {
									err("next", "invalid integer digit");
									return set_token(Token_Type::end_of_file);
								}
								integer_ = integer_ * 10 + (*i - '0');
							}
						}
						return set_token(Token_Type::integer);
					}
					while (isdigit(ch_)) {
						integer_ = integer_ * 10 + (ch_ - '0');
						get();
					}
					return set_token(Token_Type::integer);
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
