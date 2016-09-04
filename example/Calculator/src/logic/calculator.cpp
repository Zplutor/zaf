#include "logic/calculator.h"
#include <zaf/base/string/to_string.h>
#include "logic/calculate_result.h"
#include "logic/evaluate/value.h"
#include "logic/evaluate/value_evaluator.h"
#include "logic/interpret/command_interpreter.h"
#include "logic/interpret/interpret_context.h"
#include "logic/syntax_tree/format_node.h"
#include "logic/syntax_tree/length_node.h"
#include "logic/syntax_tree/operator_node.h"
#include "logic/syntax_tree/syntax_tree_builder.h"

static void GetExpressionNodeAndControlNodes(
    const std::shared_ptr<SyntaxNode>& command_node, 
    std::shared_ptr<SyntaxNode>& expression_node,
    std::shared_ptr<FormatNode>& format_node,
    std::shared_ptr<LengthNode>& length_node);

static std::shared_ptr<FormatNode> GetDefaultFormatNode();
static std::shared_ptr<LengthNode> GetDefaultLengthNode();

static void CalculateExpression(
    const std::shared_ptr<SyntaxNode>& expression_node,
    const std::shared_ptr<CalculateResult>& result,
    std::int64_t& integer_value);

static void GenerateText(
    std::int64_t value,
    const std::shared_ptr<FormatNode>& format_node,
    const std::shared_ptr<LengthNode>& length_node,
    const std::shared_ptr<CalculateResult>& result);

std::shared_ptr<CalculateResult> Calculator::Calculate(const std::wstring& command_text) {

    auto result = std::make_shared<CalculateResult>();

    InterpretContext interpret_context(command_text);
    SyntaxTreeBuilder syntax_tree_builder;
    CommandInterpreter command_interpreter;
    auto interpret_status = command_interpreter.Interpret(interpret_context, syntax_tree_builder);
    if (interpret_status != InterpretStatus::Ok) {
        result->error = CalculateResult::Error::SyntaxError;
        return result;
    }

    auto root_node = syntax_tree_builder.GetRootNode();

    std::shared_ptr<SyntaxNode> expression_node;
    std::shared_ptr<FormatNode> format_node;
    std::shared_ptr<LengthNode> length_node;
    GetExpressionNodeAndControlNodes(root_node, expression_node, format_node, length_node);
    if (expression_node == nullptr) {
        result->error = CalculateResult::Error::SemanticsError;
        return result;
    }

    std::int64_t integer_value = 0;
    CalculateExpression(expression_node, result, integer_value);
    if (result->error != CalculateResult::Error::None) {
        return result;
        
    }

    if (format_node == nullptr) {
        format_node = GetDefaultFormatNode();
    }

    if (length_node == nullptr) {
        length_node = GetDefaultLengthNode();
    }
    
    GenerateText(integer_value, format_node, length_node, result);
    return result;
}


static void GetExpressionNodeAndControlNodes(
    const std::shared_ptr<SyntaxNode>& command_node,
    std::shared_ptr<SyntaxNode>& expression_node,
    std::shared_ptr<FormatNode>& format_node,
    std::shared_ptr<LengthNode>& length_node) {

    auto operator_node = std::dynamic_pointer_cast<OperatorNode>(command_node);
    if (operator_node != nullptr && operator_node->type == OperatorNode::Type::Control) {
        
        const auto& children = operator_node->children;
        if (children.size() > 0) {
            expression_node = children[0];
        }

        for (std::size_t index = 1; index < children.size(); ++index) {

            const auto& child = children[index];

            auto casted_format_node = std::dynamic_pointer_cast<FormatNode>(child);
            if (casted_format_node != nullptr) {
                format_node = casted_format_node;
                continue;
            }
            
            auto casted_length_node = std::dynamic_pointer_cast<LengthNode>(child);
            if (casted_length_node != nullptr) {
                length_node = casted_length_node;
            }
        }
    }
    else {

        expression_node = command_node;
    }
}


static std::shared_ptr<FormatNode> GetDefaultFormatNode() {

    auto format_node = std::make_shared<FormatNode>();
    format_node->type = FormatNode::Type::Decimal;
    return format_node;
}


static std::shared_ptr<LengthNode> GetDefaultLengthNode() {

    auto length_node = std::make_shared<LengthNode>();
    length_node->type = LengthNode::Type::DoubleWord;
    return length_node;
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


static void GenerateText(
    std::int64_t value,
    const std::shared_ptr<FormatNode>& format_node,
    const std::shared_ptr<LengthNode>& length_node,
    const std::shared_ptr<CalculateResult>& result) {

    int radix = 0;
    switch (format_node->type) {
        case FormatNode::Type::Binary:
            radix = 2;
            break;
        case FormatNode::Type::Octal:
            radix = 8;
            break;
        case FormatNode::Type::Hex:
            radix = 16;
            break;
        case FormatNode::Type::Decimal:
        default:
            radix = 10;
            break;
    }

    std::wstring text;
    switch (length_node->type) {
        case LengthNode::Type::Byte:
            text = zaf::ToWideString(static_cast<std::int8_t>(value), radix);
            break;

        case LengthNode::Type::Word:
            text = zaf::ToWideString(static_cast<std::int16_t>(value), radix);
            break;

        case LengthNode::Type::DoubleWord:
            text = zaf::ToWideString(static_cast<std::int32_t>(value), radix);
            break;

        case LengthNode::Type::QuadrupleWord:
        default:
            text = zaf::ToWideString(static_cast<std::int64_t>(value), radix);
            break;
    }

    result->text = text;
}
