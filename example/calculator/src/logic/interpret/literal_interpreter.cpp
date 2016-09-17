#include "logic/interpret/literal_interpreter.h"
#include "logic/interpret/interpret_context.h"

InterpretStatus LiteralInterpreter::Interpret(InterpretContext& context, SyntaxTreeBuilder& syntax_tree_builder) {

    for (std::size_t index = 0; index < literal_.length(); ++index) {

        if (context.GetCurrentChar() != literal_[index]) {
            if (index == 0) {
                return InterpretStatus::Mismatched;
            }
            else {
                return InterpretStatus::Error;
            }
        }

        context.Forward();
    }

    return InterpretStatus::Ok;
}