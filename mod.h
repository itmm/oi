#pragma once

#include <memory>

#include "tok.h"

class Module;

std::shared_ptr<Module> read_module(Tokenizer &tok);

