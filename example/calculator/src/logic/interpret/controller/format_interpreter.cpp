#include "logic/interpret/controller/format_interpreter.h"
#include <cctype>
#include "logic/interpret/interpret_context.h"
#include "logic/interpret/rule_interpreter.h"
#include "logic/interpret/terminal_interpreter.h"
#include "logic/syntax_tree/format_node.h"
#include "logic/syntax_tree/syntax_tree_builder.h"

class FormatDescriptorInterpreter : public TerminalInterpreter {
public:
    InterpretStatus Interpret(InterpretContext& context, SyntaxTreeBuilder& syntax_tree_builder) override {

        FormatNode::Type format_type = FormatNode::Type::None;

        auto current_char = std::tolower(context.GetCurrentChar());
        switch (current_char) {

            case L'b':
                format_type = FormatNode::Type::Binary;
                break;

            case L'o':
                format_type = FormatNode::Type::Octal;
                break;

            case L'd':
                format_type = FormatNode::Type::Decimal;
                break;

            case L'x':
                format_type = FormatNode::Type::Hex;
                break;

            default:
                return InterpretStatus::Mismatched;
        }

        auto format_node = std::make_shared<FormatNode>();
        format_node->type = format_type;
        syntax_tree_builder.AddOperand(format_node);

        context.Forward();
        return InterpretStatus::Ok;
    }
};


FormatInterpreter::FormatInterpreter() {

    AddRuleInterpreter(CreateRuleInterpreter({ std::make_shared<FormatDescriptorInterpreter>() }));
    AddRuleInterpreter(CreateRuleInterpreter({ }));
}