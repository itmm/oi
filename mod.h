#pragma once

#include <map>
#include <memory>
#include <string>

#include "err.h"
#include "mapping.h"
#include "obj.h"
#include "statement.h"
#include "tok.h"

class Module: public Obj {
		const std::string name_;
		static std::map<std::string, std::shared_ptr<Module>> all_modules_;

	public:
		Module(std::string name): name_ { name } { }

		Mapping mapping;
		std::shared_ptr<Statement::List> init;

		const std::string &name() const { return name_; }
		
		static std::shared_ptr<Module> find(const std::string &name) {
			auto got { all_modules_.find(name) };
			return got != all_modules_.end() ? got->second : nullptr;
		}

		static std::shared_ptr<Module> create(const std::string &name) {
			if (find(name)) { err("create_module", "duplicates"); return nullptr; }
			auto result { std::make_shared<Module>(name) };
			all_modules_.emplace(name, result);
			return result;
		}
};


std::shared_ptr<Module> read_module(Tokenizer &tok);

