#include "logic/evaluate/value_evaluator.h"
#include "logic/evaluate/operand_evaluator.h"
#include "logic/evaluate/operator_evaluator.h"
#include "logic/syntax_tree/operand_node.h"
#include "logic/syntax_tree/operator_node.h"

std::shared_ptr<ValueEvaluator> CreateValueEvaluator(const std::shared_ptr<SyntaxNode>& syntax_node) {

    auto operand_node = std::dynamic_pointer_cast<OperandNode>(syntax_node);
    if (operand_node != nullptr) {

        auto evaluator = std::make_shared<OperandEvaluator>();
        evaluator->SetSyntaxNode(operand_node);
        return evaluator;
    }

    auto operator_node = std::dynamic_pointer_cast<OperatorNode>(syntax_node);
    if (operator_node != nullptr) {

        auto evaluator = CreateOperatorEvaluator(operator_node);
        evaluator->SetSyntaxNode(operator_node);
        return evaluator;
    }

    return nullptr;
}