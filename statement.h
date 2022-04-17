#pragma once

#include "statement.h"

#include "exp.h"
#include "tok.h"
#include "obj.h"

#include <memory>
#include <vector>

namespace Statement {
	class Statement: public Obj {
		public:
			static std::shared_ptr<Statement> read(Tokenizer &tok);
	};

	class Empty: public Statement { };

	class List: public Statement {
			std::vector<std::shared_ptr<Statement>> statements_;
		public:
			static std::shared_ptr<List> read(Tokenizer &tok);
	};

	struct If_Clause {
		std::shared_ptr<Expression::Expression> cond;
		std::shared_ptr<List> block;

		static If_Clause read(Tokenizer &tok);
	};

	class If: public Statement {
			If_Clause base_;
			std::vector<If_Clause> elsifs_;
			std::shared_ptr<List> else_;
		public:
			static std::shared_ptr<If> read(Tokenizer &tok);
	};

	struct Do_Clause {
		std::shared_ptr<Expression::Expression> cond;
		std::shared_ptr<List> block;

		static Do_Clause read(Tokenizer &tok);
	};

	class While: public Statement {
			Do_Clause base_;
			std::vector<Do_Clause> elsifs_;
		public:
			static std::shared_ptr<While> read(Tokenizer &tok);
	};
}
