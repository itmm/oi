#include "statement.h"

#include "err.h"

namespace Statement {
	std::shared_ptr<List> List::read(Tokenizer &tok) {
		auto me { std::make_shared<List>() };
		for (;;) {
			auto cur { Statement::read(tok) };
			if (! cur) { err("statement_list", "can't read statement"); return nullptr; }
			me->statements_.push_back(cur);
			if (tok.type() != Token_Type::semicolon) { break; }
			tok.next();
		}
		return me;
	}

	If_Clause If_Clause::read(Tokenizer &tok) {
		If_Clause result;
		result.cond = Expression::Expression::read(tok);
		assert_next_tok(Token_Type::then_kw, tok, "if", "'THEN' expected");
		result.block = List::read(tok);
		return result;

	}

	std::shared_ptr<If> If::read(Tokenizer &tok) {
		auto res { std::make_shared<If>() };
		res->base_ = If_Clause::read(tok);
		while (tok.type() == Token_Type::elsif_kw) {
			tok.next();
			res->elsifs_.push_back(If_Clause::read(tok));
		}
		if (tok.type() == Token_Type::else_kw) {
			tok.next();
			res->else_ = List::read(tok);
		}
		assert_next_tok(Token_Type::end_kw, tok, "if", "'END expected");
		return res;
	}

	Do_Clause Do_Clause::read(Tokenizer &tok) {
		Do_Clause result;
		result.cond = Expression::Expression::read(tok);
		assert_next_tok(Token_Type::do_kw, tok, "while", "'DO' expected");
		result.block = List::read(tok);
		return result;

	}

	std::shared_ptr<While> While::read(Tokenizer &tok) {
		auto res { std::make_shared<While>() };
		res->base_ = Do_Clause::read(tok);
		while (tok.type() == Token_Type::elsif_kw) {
			tok.next();
			res->elsifs_.push_back(Do_Clause::read(tok));
		}
		assert_next_tok(Token_Type::end_kw, tok, "while", "'END expected");
		return res;
	}

	std::shared_ptr<Statement> Statement::read(Tokenizer &tok) {
		if (tok.type() == Token_Type::semicolon) { 
			return std::make_shared<Empty>(); 
		}
		
		if (tok.type() == Token_Type::if_kw) {
			tok.next();
			return If::read(tok);
		}
		if (tok.type() == Token_Type::while_kw) {
			tok.next();
			return While::read(tok);
		}
		// TODO: Assignment
		// TODO: Procedure Call
		// TODO: Case Statement
		// TODO: Repeat Statement
		// TODO: For Statement
		err("statement", "unknown statement");
		return nullptr;
	}
}
