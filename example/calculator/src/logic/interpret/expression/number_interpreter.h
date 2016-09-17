#pragma once

#include "logic/interpret/terminal_interpreter.h"

class NumberInterpreter : public TerminalInterpreter {
public:
    InterpretStatus Interpret(InterpretContext& context, SyntaxTreeBuilder& syntax_tree_builder) override;
};