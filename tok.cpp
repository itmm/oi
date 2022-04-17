#include "tok.h"

std::map<std::string, Token_Type> Tokenizer::keywords_ {
	{ "BEGIN", Token_Type::begin_kw },
	{ "CONST", Token_Type::const_kw },
	{ "DIV", Token_Type::div_kw },
	{ "DO", Token_Type::do_kw },
	{ "ELSE", Token_Type::else_kw },
	{ "ELSIF", Token_Type::elsif_kw },
	{ "END", Token_Type::end_kw },
	{ "FALSE", Token_Type::false_kw },
	{ "IF", Token_Type::if_kw },
	{ "IMPORT", Token_Type::import_kw },
	{ "MOD", Token_Type::mod_kw },
	{ "MODULE", Token_Type::module_kw },
	{ "NIL", Token_Type::nil_kw },
	{ "PROCEDURE", Token_Type::procedure_kw },
	{ "OR", Token_Type::or_kw },
	{ "THEN", Token_Type::then_kw },
	{ "TRUE", Token_Type::true_kw },
	{ "TYPE", Token_Type::type_kw },
	{ "VAR", Token_Type::var_kw },
	{ "WHILE", Token_Type::while_kw }
};

Token_Type Tokenizer::get_real() {
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
}

Token_Type Tokenizer::get_integer() {
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

Token_Type Tokenizer::get_number() {
	bool is_real { ch_ == '.' };
	if (is_real) {
		return get_real();
	} else {
		return get_integer();
	}
}

Token_Type Tokenizer::get_ident() {
	ident_.clear();
	while (isalnum(ch_)) { ident_ += static_cast<char>(ch_); get(); }
	auto got { keywords_.find(ident_) };
	if (got != keywords_.end()) {
		return set_token(got->second);
	}
	if (isdigit(ident_[0])) {
		return get_number();
	}
	return set_token(Token_Type::identifier);
}

void Tokenizer::read_comment() {
	for (;;) {
		if (ch_ == '(') {
			get();
			if (ch_ == '*') { read_comment(); }
		} else if (ch_ == '*') {
			get();
			if (ch_ == ')') { get(); break; }
		} else { get(); }
	}
}

Token_Type Tokenizer::next() {
	while (ch_ != EOF && ch_ <= ' ') { get(); }
	if (isalnum(ch_)) {
		return get_ident();
	} else switch (ch_) {
		case EOF: return set_token(Token_Type::end_of_file);
		case ';': return get_and_set_token(Token_Type::semicolon);
		case '.': return get_and_set_token(Token_Type::period);
		case ':': {
			get();
			if (ch_ == '=') {
				return get_and_set_token(Token_Type::becomes);
			}
			return set_token(Token_Type::colon);
		}
		case ',': return get_and_set_token(Token_Type::comma);
		case '*': return get_and_set_token(Token_Type::asterisk);
		case '=': return get_and_set_token(Token_Type::equals);
		case '+': return get_and_set_token(Token_Type::plus);
		case '-': return get_and_set_token(Token_Type::minus);
		case '/': return get_and_set_token(Token_Type::div);
		case '&': return get_and_set_token(Token_Type::and_sym);
		case '(': {
			get();
			if (ch_ == '*') {
				read_comment();
				return next();
			}
			return set_token(Token_Type::lparen);
		}
		case ')': return get_and_set_token(Token_Type::rparen);
		default:
			  err("token", "unknown char '"s + static_cast<char>(ch_) + "'"s);
			  throw 10;
	}
}
