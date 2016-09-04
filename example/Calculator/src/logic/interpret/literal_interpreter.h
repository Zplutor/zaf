#pragma once

#include <string>
#include "logic/interpret/symbol_interpreter.h"

class LiteralInterpreter : public SymbolInterpreter {
public:
    LiteralInterpreter(const std::wstring& literal) : literal_(literal) { }

    InterpretStatus Interpret(InterpretContext& context, SyntaxTreeBuilder& syntax_tree_builder) override;

private:
    std::wstring literal_;
};