#include "logic/interpret/controller/controller_interpreter.h"
#include <cctype>
#include "logic/interpret/controller/format_interpreter.h"
#include "logic/interpret/controller/length_interpreter.h"
#include "logic/interpret/interpret_context.h"
#include "logic/interpret/literal_interpreter.h"
#include "logic/interpret/rule_interpreter.h"
#include "logic/interpret/terminal_interpreter.h"
#include "logic/syntax_tree/operator_node.h"
#include "logic/syntax_tree/syntax_tree_builder.h"

class ControlOperatorInterpreter : public TerminalInterpreter {
public:
    InterpretStatus Interpret(InterpretContext& context, SyntaxTreeBuilder& syntax_tree_builder) override {

        if (std::tolower(context.GetCurrentChar()) == L'=') {

            auto operator_node = std::make_shared<OperatorNode>();
            operator_node->type = OperatorNode::Type::Control;
            syntax_tree_builder.AddOperator(operator_node);

            context.Forward();
            return InterpretStatus::Ok;
        }
        else {
            return InterpretStatus::Mismatched;
        }
    }
};


ControllerInterpreter::ControllerInterpreter() {

    auto control_descriptor1_interpreter = std::make_shared<NonTerminalInterpreter>();
    control_descriptor1_interpreter->AddRuleInterpreter(CreateRuleInterpreter({
        std::make_shared<LiteralInterpreter>(L","),
        std::make_shared<LengthInterpreter>()
    }));
    control_descriptor1_interpreter->AddRuleInterpreter(CreateRuleInterpreter({ }));

    auto control_descriptor_interpreter = std::make_shared<NonTerminalInterpreter>();
    control_descriptor_interpreter->AddRuleInterpreter(CreateRuleInterpreter({ 
        std::make_shared<FormatInterpreter>(),
        control_descriptor1_interpreter,
    }));

    AddRuleInterpreter(CreateRuleInterpreter({ 
        std::make_shared<ControlOperatorInterpreter>(),
        control_descriptor_interpreter 
    }));

    AddRuleInterpreter(CreateRuleInterpreter({ }));
}