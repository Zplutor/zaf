#include "logic/interpret/non_terminal_interpreter.h"
#include "logic/interpret/rule_interpreter.h"

InterpretStatus NonTerminalInterpreter::Interpret(InterpretContext& context, SyntaxTreeBuilder& syntax_tree_builder) {

    for (const auto& each_interpreter : rule_interpreters_) {

        auto status = each_interpreter->Interpret(context, syntax_tree_builder);
        if (status != InterpretStatus::Mismatched) {
            return status;
        }
    }

    return InterpretStatus::Mismatched;
}