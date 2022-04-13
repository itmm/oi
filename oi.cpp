#include "token.h"

int main(int argc, const char *argv[]) {
	Tokenizer tok(std::cin);
	while (tok.type() != Token_Type::end_of_file) {
		tok.next();
	}
}
