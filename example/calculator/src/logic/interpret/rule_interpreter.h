#pragma once

#include <memory>
#include <vector>
#include "logic/interpret/interpreter.h"

class SymbolInterpreter;

class RuleInterpreter : public Interpreter {
public:
    void AddSymbolInterpreter(const std::shared_ptr<SymbolInterpreter>& symbol_interpreter) {
        symbol_interpreters_.push_back(symbol_interpreter);
    }

    InterpretStatus Interpret(InterpretContext& context, SyntaxTreeBuilder& syntax_tree_builder) override;

private:
    std::vector<std::shared_ptr<SymbolInterpreter>> symbol_interpreters_;
};


std::shared_ptr<RuleInterpreter> CreateRuleInterpreter(
    const std::initializer_list<std::shared_ptr<SymbolInterpreter>>& symbol_interpreters);