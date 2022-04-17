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
			static std::shared_ptr<Expression> read(Tokenizer &tok);
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
}
