#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>

#include "tok.h"

namespace Const {
	class Value {
		public:
			virtual ~Value() { }
	};

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

	class Mapping {
			std::map<std::string, std::unique_ptr<Value>> values_;
			std::set<std::string> exports_;
			
		public:
			void add(const std::string &name, std::unique_ptr<Value>, bool exported);
			Value *get(const std::string &name, bool exported_only) const;
	};

	bool is_declaration_start(Tokenizer &tok);
	void read_declaration(Mapping &mapping, Tokenizer &tok);
}
