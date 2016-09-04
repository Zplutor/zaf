#include "logic/interpret/controller/length_interpreter.h"
#include <cctype>
#include "logic/interpret/interpret_context.h"
#include "logic/interpret/rule_interpreter.h"
#include "logic/interpret/terminal_interpreter.h"
#include "logic/syntax_tree/length_node.h"
#include "logic/syntax_tree/syntax_tree_builder.h"

class LengthDescriptorInterpreter : public TerminalInterpreter {
public:
    InterpretStatus Interpret(InterpretContext& context, SyntaxTreeBuilder& syntax_tree_builder) override {

        auto length_type = LengthNode::Type::None;

        wchar_t current_char = std::tolower(context.GetCurrentChar());
        switch (current_char) {

            case L'b':
                length_type = LengthNode::Type::Byte;
                break;

            case L'w':
                length_type = LengthNode::Type::Word;
                break;

            case L'd':
                length_type = LengthNode::Type::DoubleWord;
                break;

            case L'q':
                length_type = LengthNode::Type::QuadrupleWord;
                break;

            default:
                return InterpretStatus::Mismatched;
        }

        auto length_node = std::make_shared<LengthNode>();
        length_node->type = length_type;
        syntax_tree_builder.AddOperand(length_node);
    
        context.Forward();
        return InterpretStatus::Ok;
    }
};


LengthInterpreter::LengthInterpreter() {

    AddRuleInterpreter(CreateRuleInterpreter({ std::make_shared<LengthDescriptorInterpreter>() }));
    AddRuleInterpreter(CreateRuleInterpreter({ }));
}