#include "mapping.h"
#include "err.h"

void Mapping::add(const std::string &name, std::shared_ptr<Obj> value, bool exported) {
	values_[name] = value;
	if (exported) {
		if (! exports_) { err("mapping", "exprot not allowed for '" + name + "'"); }
		exports_->insert(name);
	}
}

std::shared_ptr<Obj> Mapping::get(const std::string &name, bool exported_only) const {
	auto got { values_.find(name) };
	if (got == values_.end() || (exported_only && exports_ && exports_->find(name) == exports_->end())) {
		return nullptr;
	}
	return got->second;
}
