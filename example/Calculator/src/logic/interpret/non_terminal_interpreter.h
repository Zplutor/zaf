#pragma once

#include <memory>
#include <vector>
#include "logic/interpret/symbol_interpreter.h"

class RuleInterpreter;

class NonTerminalInterpreter : public SymbolInterpreter {
public:
    void AddRuleInterpreter(const std::shared_ptr<RuleInterpreter>& rule_interpreter) {
        rule_interpreters_.push_back(rule_interpreter);
    }

    InterpretStatus Interpret(InterpretContext& context, SyntaxTreeBuilder& syntax_tree_builder) override;

private:
    std::vector<std::shared_ptr<RuleInterpreter>> rule_interpreters_;
};