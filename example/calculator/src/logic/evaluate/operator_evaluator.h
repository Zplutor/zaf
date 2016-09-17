#pragma once

#include "logic/evaluate/value_evaluator.h"

class OperatorNode;

class OperatorEvaluator : public BaseValueEvaluator<OperatorNode> {
public:
    EvaluateStatus Evaluate(std::shared_ptr<Value>& value) override {

        auto status = EvaluateChildren();
        if (status != EvaluateStatus::Ok) {
            return status;
        }

        return EvaluateValue(value);
    }

protected:
    virtual EvaluateStatus EvaluateChildren() = 0;
    virtual EvaluateStatus EvaluateValue(std::shared_ptr<Value>& value) = 0;
};

std::shared_ptr<OperatorEvaluator> CreateOperatorEvaluator(const std::shared_ptr<OperatorNode>& operator_node);