#pragma once

#include <memory>
#include "logic/interpret/interpret_status.h"

class InterpretContext;
class SyntaxTreeBuilder;

class Interpreter {
public:
    virtual ~Interpreter() { }
    virtual InterpretStatus Interpret(InterpretContext& context, SyntaxTreeBuilder& syntax_tree_builder) = 0;
};