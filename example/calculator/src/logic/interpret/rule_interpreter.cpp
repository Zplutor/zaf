#include "logic/interpret/rule_interpreter.h"
#include <cctype>
#include "logic/interpret/interpret_context.h"
#include "logic/interpret/symbol_interpreter.h"

InterpretStatus RuleInterpreter::Interpret(InterpretContext& context, SyntaxTreeBuilder& syntax_tree_builder) {

    for (const auto& each_interpreter : symbol_interpreters_) {

        auto status = each_interpreter->Interpret(context, syntax_tree_builder);
        if (status == InterpretStatus::Error) {
            return status;
        }

        if (status == InterpretStatus::Mismatched) {
            if (each_interpreter == symbol_interpreters_.front()) {
                return status;
            }
            else {
                return InterpretStatus::Error;
            }
        }

        while (std::isspace(context.GetCurrentChar())) {
            if (! context.Forward()) {
                break;
            }
        }
    }

    return InterpretStatus::Ok;
}


std::shared_ptr<RuleInterpreter> CreateRuleInterpreter(
    const std::initializer_list<std::shared_ptr<SymbolInterpreter>>& symbol_interpreters) {

    auto rule_interpreter = std::make_shared<RuleInterpreter>();
    for (const auto& each_interpreter : symbol_interpreters) {
        rule_interpreter->AddSymbolInterpreter(each_interpreter);
    }
    return rule_interpreter;
}