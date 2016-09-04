#include "logic/interpret/expression/binary_expression_interpreter.h"
#include "logic/interpret/expression/expression_interpreter.h"
#include "logic/interpret/interpret_context.h"
#include "logic/interpret/rule_interpreter.h"
#include "logic/interpret/terminal_interpreter.h"
#include "logic/syntax_tree/operator_node.h"
#include "logic/syntax_tree/syntax_tree_builder.h"

class BinaryOperatorInterpreter : public TerminalInterpreter {
public:
    InterpretStatus Interpret(InterpretContext& context, SyntaxTreeBuilder& syntax_tree_builder) override {

        OperatorNode::Type type = OperatorNode::Type::None;

        switch (context.GetCurrentChar()) {

            case L'+':
                type = OperatorNode::Type::Plus;
                break;

            case L'-':
                type = OperatorNode::Type::Minus;
                break;

            case L'*':
                type = OperatorNode::Type::Multiply;
                break;

            case L'/':
                type = OperatorNode::Type::Divide;
                break;

            default:
                return InterpretStatus::Mismatched;
        }

        auto operator_node = std::make_shared<OperatorNode>();
        operator_node->type = type;
        syntax_tree_builder.AddOperator(operator_node);

        if (context.Forward()) {
            return InterpretStatus::Ok;
        }
        else {
            return InterpretStatus::Error;
        }
    }
};


BinaryExpressionInterpreter::BinaryExpressionInterpreter() {

    AddRuleInterpreter(CreateRuleInterpreter({
        std::make_shared<BinaryOperatorInterpreter>(),
        std::make_shared<ExpressionInterpreter>() }));

    AddRuleInterpreter(CreateRuleInterpreter({}));
}
