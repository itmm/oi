#pragma once

#include "obj.h"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

using Exports = std::set<std::string>;

class Mapping {
		std::map<std::string, std::shared_ptr<Obj>> values_;
		Exports *exports_;
	public:
		Mapping(Exports *exports = nullptr): exports_ { exports } { }
		void add(const std::string &name, std::shared_ptr<Obj>, bool exported);
		std::shared_ptr<Obj> get(const std::string &name, bool exported_only) const;
		bool has(const std::string &name) const { return get(name, false) != nullptr; };
};

