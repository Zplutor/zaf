#pragma once

#include "logic/evaluate/value_evaluator.h"

class OperandNode;

class OperandEvaluator : public BaseValueEvaluator<OperandNode> {
public:
    EvaluateStatus Evaluate(std::shared_ptr<Value>& value) override;
};