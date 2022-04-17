#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>

#include "tok.h"
#include "obj.h"
#include "mapping.h"

namespace Expression {
	class Expression: public Obj {
		public:
			static std::shared_ptr<Expression> read(const Mapping &mapping, Tokenizer &tok);
			virtual std::shared_ptr<Expression> eval(std::shared_ptr<Expression> me, const Mapping &mapping) { return me; }
	};

	template<typename TYPE, typename BASE> class Concrete: public BASE {
			const TYPE value_;
		public:
			Concrete(TYPE v): value_ { v } { }
			TYPE value() const { return value_; }
	};

	class Numeric: public Expression { };
	using Integer = Concrete<int, Numeric>;
	using Real = Concrete<double, Numeric>;
	using Bool = Concrete<bool, Expression>;
	using String = Concrete<std::string, Expression>;

	inline bool is_bool(std::shared_ptr<Expression> exp) {
		return dynamic_cast<Bool *>(exp.get()) != nullptr;
	}

	inline bool is_true(std::shared_ptr<Expression> exp) {
		auto b { dynamic_cast<Bool *>(exp.get()) };
		return b->value();
	}

	class Variable: public Expression {
			std::string name_;
		public:
			Variable(std::string name): name_ { name } { }
	};

	class Unary: public Expression {
			Token_Type op_;
			std::shared_ptr<Expression> first_;
		public:
			Unary(Token_Type op, std::shared_ptr<Expression> first): op_ { op }, first_ { first } { }
	};

	class Binary: public Expression {
			std::shared_ptr<Expression> first_;
			Token_Type op_;
			std::shared_ptr<Expression> second_;
		public:
			Binary(std::shared_ptr<Expression> first, Token_Type op, std::shared_ptr<Expression> second): first_ { first }, op_ { op }, second_ { second } { }
	};
}
