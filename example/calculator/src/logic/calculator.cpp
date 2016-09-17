#include "logic/calculator.h"
#include <zaf/base/string/to_string.h>
#include "logic/calculate_result.h"
#include "logic/evaluate/value.h"
#include "logic/evaluate/value_evaluator.h"
#include "logic/interpret/expression/expression_interpreter.h"
#include "logic/interpret/interpret_context.h"
#include "logic/syntax_tree/operator_node.h"
#include "logic/syntax_tree/syntax_tree_builder.h"

static void CalculateExpression(
    const std::shared_ptr<SyntaxNode>& expression_node,
    const std::shared_ptr<CalculateResult>& result,
    std::int64_t& integer_value);

std::shared_ptr<CalculateResult> Calculator::Calculate(const std::wstring& command_text) {

    auto result = std::make_shared<CalculateResult>();

    InterpretContext interpret_context(command_text);
    SyntaxTreeBuilder syntax_tree_builder;
    ExpressionInterpreter expression_interpreter;
    auto interpret_status = expression_interpreter.Interpret(interpret_context, syntax_tree_builder);
    if (interpret_status != InterpretStatus::Ok) {
        result->error = CalculateResult::Error::SyntaxError;
        return result;
    }

    auto root_node = syntax_tree_builder.GetRootNode();

    std::int64_t integer_value = 0;
    CalculateExpression(root_node, result, integer_value);
    if (result->error != CalculateResult::Error::None) {
        return result;
        
    }

    result->text = zaf::ToWideString(integer_value);
    return result;
}


static void CalculateExpression(
    const std::shared_ptr<SyntaxNode>& expression_node,
    const std::shared_ptr<CalculateResult>& result, 
    std::int64_t& integer_value) {

    auto evaluator = CreateValueEvaluator(expression_node);
    if (evaluator == nullptr) {
        result->error = CalculateResult::Error::SemanticsError;
        return;
    }

    std::shared_ptr<Value> value;
    auto evaluate_status = evaluator->Evaluate(value);
    if (evaluate_status != EvaluateStatus::Ok) {
        result->error = CalculateResult::Error::SemanticsError;
        return;
    }
    
    integer_value = value->integer_value;
}

