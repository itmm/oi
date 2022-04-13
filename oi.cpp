#include "tok.h"
#include "mod.h"

int main(int argc, const char *argv[]) {
	Tokenizer tok(std::cin);
	read_module(tok);
	assert_tok(Token_Type::end_of_file, tok, "main", "data after MODULE");
}
