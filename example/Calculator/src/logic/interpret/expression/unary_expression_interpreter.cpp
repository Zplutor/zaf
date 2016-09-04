#include "logic/interpret/expression/unary_expression_interpreter.h"
#include "logic/interpret/expression/expression_interpreter.h"
#include "logic/interpret/interpret_context.h"
#include "logic/interpret/rule_interpreter.h"
#include "logic/interpret/terminal_interpreter.h"
#include "logic/syntax_tree/operator_node.h"
#include "logic/syntax_tree/syntax_tree_builder.h"

class UnaryOperatorInterpreter : public TerminalInterpreter {
public:
    InterpretStatus Interpret(InterpretContext& context, SyntaxTreeBuilder& syntax_tree_builder) override {

        OperatorNode::Type operator_type = OperatorNode::Type::None;

        wchar_t current_char = context.GetCurrentChar();
        switch (current_char) {

        case L'+':
            operator_type = OperatorNode::Type::Positive;
            break;

        case L'-':
            operator_type = OperatorNode::Type::Negative;
            break;

        case L'~':
            operator_type = OperatorNode::Type::Not;
            break;

        default:
            return InterpretStatus::Mismatched;
        }

        auto operator_node = std::make_shared<OperatorNode>();
        operator_node->type = operator_type;
        syntax_tree_builder.AddOperator(operator_node);

        if (context.Forward()) {
            return InterpretStatus::Ok;
        }
        else {
            return InterpretStatus::Error;
        }
    }
};


UnaryExpressionInterpreter::UnaryExpressionInterpreter() {

    AddRuleInterpreter(CreateRuleInterpreter({ 
        std::make_shared<UnaryOperatorInterpreter>(),
        std::make_shared<ExpressionInterpreter>() }));
}