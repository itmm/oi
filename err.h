#pragma once

#include <stdexcept>

using namespace std::literals::string_literals;

class Err : public std::exception {
		std::string reason_;
	public:
		Err(const std::string &reason) : reason_ { reason } { }
		const char *what() const throw() override { return reason_.c_str(); }
};

inline void err(const std::string &from, const std::string &reason) {
	throw Err { from + ": " + reason };
}
