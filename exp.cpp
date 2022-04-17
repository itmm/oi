#include "exp.h"

#include "err.h"

namespace Expression {

	std::shared_ptr<Expression> Expression::read(Tokenizer &tok) {
		err("expression", "not implemented");
		return nullptr;
	}

}
