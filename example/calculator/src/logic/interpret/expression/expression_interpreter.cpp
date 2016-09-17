#include "logic/interpret/expression/expression_interpreter.h"
#include "logic/interpret/expression/binary_expression_interpreter.h"
#include "logic/interpret/expression/number_interpreter.h"
#include "logic/interpret/expression/unary_expression_interpreter.h"
#include "logic/interpret/interpret_context.h"
#include "logic/interpret/rule_interpreter.h"
#include "logic/interpret/terminal_interpreter.h"
#include "logic/syntax_tree/operator_node.h"
#include "logic/syntax_tree/syntax_tree_builder.h"

class ParenthesisInterpreter : public TerminalInterpreter {
public:
    ParenthesisInterpreter(OperatorNode::Type operator_type) : operator_type_(operator_type) { }

    InterpretStatus Interpret(InterpretContext& context, SyntaxTreeBuilder& builder) override {

        if (context.GetCurrentChar() == GetParenthesisChar()) {

            auto operator_node = std::make_shared<OperatorNode>();
            operator_node->type = operator_type_;
            builder.AddOperator(operator_node);
            context.Forward();
            return InterpretStatus::Ok;
        }
        else {
            return InterpretStatus::Mismatched;
        }
    }

private:
    wchar_t GetParenthesisChar() const {
        return operator_type_ == OperatorNode::Type::LeftParenthesis ? L'(' : L')';
    }

private:
    OperatorNode::Type operator_type_;
};


ExpressionInterpreter::ExpressionInterpreter() {

    AddRuleInterpreter(CreateRuleInterpreter({ 
        std::make_shared<NumberInterpreter>(), 
        std::make_shared<SymbolProxyInterpreter<BinaryExpressionInterpreter>>() }));

    AddRuleInterpreter(CreateRuleInterpreter({ 
        std::make_shared<SymbolProxyInterpreter<UnaryExpressionInterpreter>>(),
        std::make_shared<SymbolProxyInterpreter<BinaryExpressionInterpreter>>() }));

    AddRuleInterpreter(CreateRuleInterpreter({ 
        std::make_shared<ParenthesisInterpreter>(OperatorNode::Type::LeftParenthesis),
        std::make_shared<SymbolProxyInterpreter<ExpressionInterpreter>>(),
        std::make_shared<ParenthesisInterpreter>(OperatorNode::Type::RightParenthesis),
        std::make_shared<SymbolProxyInterpreter<BinaryExpressionInterpreter>>() }));
}