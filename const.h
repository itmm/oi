#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>

#include "tok.h"
#include "exp.h"
#include "mapping.h"

namespace Const {
	class Value: public Expression::Expression {
		public:
			static bool is_start(Tokenizer &tok);
			static void read(Mapping &mapping, Tokenizer &tok);
	};

	template<typename TYPE, typename BASE> class Concrete: public BASE {
			const TYPE value_;
		public:
			Concrete(TYPE v): value_ { v } { }
			TYPE value() const { return value_; }
	};

	class Numeric: public Value { };
	using Integer = Concrete<int, Numeric>;
	using Real = Concrete<double, Numeric>;
	using Bool = Concrete<bool, Value>;
	using String = Concrete<std::string, Value>;
}
