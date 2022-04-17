#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>

#include "tok.h"
#include "obj.h"
#include "mapping.h"

namespace Const {
	class Value: public Obj { };

	template<typename TYPE, typename BASE> class Concrete_Value: public BASE {
			const TYPE value_;
		public:
			Concrete_Value(TYPE v): value_ { v } { }
			TYPE value() const { return value_; }
	};

	class Numeric_Value: public Value { };
	using Int_Value = Concrete_Value<int, Numeric_Value>;
	using Real_Value = Concrete_Value<double, Numeric_Value>;
	using Bool_Value = Concrete_Value<bool, Value>;
	using String_Value = Concrete_Value<std::string, Value>;

	bool is_declaration_start(Tokenizer &tok);
	void read_declaration(Mapping &mapping, Tokenizer &tok);
}
