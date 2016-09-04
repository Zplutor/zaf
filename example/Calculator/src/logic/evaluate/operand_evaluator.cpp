#include "logic/evaluate/operand_evaluator.h"
#include <cctype>
#include <zaf/base/string/to_numeric.h>
#include "logic/evaluate/value.h"
#include "logic/syntax_tree/operand_node.h"

EvaluateStatus OperandEvaluator::Evaluate(std::shared_ptr<Value>& value) {

    std::int64_t integer_value = 0;
    bool is_succeeded = zaf::TryToNumeric(GetSyntaxNode()->text, integer_value, GetSyntaxNode()->radix);
    if (! is_succeeded) {
        return EvaluateStatus::Overflow;
    }

    value = std::make_shared<Value>();
    value->type = Value::Type::Integer;
    value->integer_value = integer_value;

    return EvaluateStatus::Ok;
}