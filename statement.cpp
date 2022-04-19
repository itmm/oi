#include "statement.h"

#include "err.h"
#include "ident.h"

namespace Statement {
	std::shared_ptr<List> List::read(const Mapping &mapping, Tokenizer &tok) {
		auto me { std::make_shared<List>() };
		for (;;) {
			if (tok.type() == Token_Type::end_kw) { break; }
			auto cur { Statement::read(mapping, tok) };
			if (! cur) { err("statement_list", "can't read statement"); return nullptr; }
			me->statements_.push_back(cur);
			if (tok.type() != Token_Type::semicolon) { break; }
			tok.next();
		}
		return me;
	}

	void List::eval(Mapping &mapping) {
		for (auto &s : statements_) {
			s->eval(mapping);
		}
	}

	If_Clause If_Clause::read(const Mapping &mapping, Tokenizer &tok) {
		If_Clause result;
		result.cond = Expression::Expression::read(mapping, tok);
		assert_next_tok(Token_Type::then_kw, tok, "if", "'THEN' expected");
		result.block = List::read(mapping, tok);
		return result;

	}

	bool If_Clause::eval(Mapping &mapping) {
		if (! cond) { err("if_clause", "no cond"); return true; }
		if (! block) { err("if_clause", "no block"); return true; }
		auto c { cond->eval(cond, mapping) };
		if (! Expression::is_bool(c)) { err("if_clause", "no bool cond"); return true; }
		if (Expression::is_true(c)) {
			block->eval(mapping);
			return true;
		}
		return false;
	}

	std::shared_ptr<If> If::read(const Mapping &mapping, Tokenizer &tok) {
		auto res { std::make_shared<If>() };
		res->base_ = If_Clause::read(mapping, tok);
		while (tok.type() == Token_Type::elsif_kw) {
			tok.next();
			res->elsifs_.push_back(If_Clause::read(mapping, tok));
		}
		if (tok.type() == Token_Type::else_kw) {
			tok.next();
			res->else_ = List::read(mapping, tok);
		}
		assert_next_tok(Token_Type::end_kw, tok, "if", "'END expected");
		return res;
	}

	void If::eval(Mapping &mapping) {
		if (base_.eval(mapping)) { return; }
		for (auto &ei : elsifs_) {
			if (ei.eval(mapping)) { return; }
		}
		if (else_) { else_->eval(mapping); }
	}

	Do_Clause Do_Clause::read(const Mapping &mapping, Tokenizer &tok) {
		Do_Clause result;
		result.cond = Expression::Expression::read(mapping, tok);
		assert_next_tok(Token_Type::do_kw, tok, "while", "'DO' expected");
		result.block = List::read(mapping, tok);
		return result;

	}

	bool Do_Clause::eval(Mapping &mapping) {
		if (! cond) { err("do_clause", "no cond"); return true; }
		if (! block) { err("do_clause", "no block"); return true; }
		auto c { cond->eval(cond, mapping) };
		if (! Expression::is_bool(c)) { err("do_clause", "no bool cond"); return true; }
		if (Expression::is_true(c)) {
			block->eval(mapping);
			return true;
		}
		return false;
	}

	std::shared_ptr<While> While::read(const Mapping &mapping, Tokenizer &tok) {
		auto res { std::make_shared<While>() };
		res->base_ = Do_Clause::read(mapping, tok);
		while (tok.type() == Token_Type::elsif_kw) {
			tok.next();
			res->elsifs_.push_back(Do_Clause::read(mapping, tok));
		}
		assert_next_tok(Token_Type::end_kw, tok, "while", "'END expected");
		return res;
	}

	void While::eval(Mapping &mapping) {
		for (;;) {
			if (base_.eval(mapping)) { continue; }
			bool cont { false };
			for (auto &ei : elsifs_) {
				if (ei.eval(mapping)) { cont = true; break; }
			}
			if (! cont) { return; }
		}
	}

	std::shared_ptr<Statement> Statement::read(const Mapping &mapping, Tokenizer &tok) {
		if (tok.type() == Token_Type::semicolon) { 
			return std::make_shared<Empty>(); 
		}
		
		if (tok.type() == Token_Type::if_kw) {
			tok.next();
			return If::read(mapping, tok);
		}
		if (tok.type() == Token_Type::while_kw) {
			tok.next();
			return While::read(mapping, tok);
		}
		// TODO: Assignment
		// TODO: Procedure Call
		// TODO: Case Statement
		// TODO: Repeat Statement
		// TODO: For Statement

		auto qual_ident { Qual_Ident::read(mapping, tok) };


		err("statement", "unknown statement");
		return nullptr;
	}
}
