#include "logic/evaluate/operator_evaluator.h"
#include "logic/evaluate/value.h"
#include "logic/syntax_tree/operator_node.h"

class UnaryOperatorEvaluator : public OperatorEvaluator {
public:
    EvaluateStatus EvaluateChildren() override {

        auto child_evalutor = CreateValueEvaluator(GetSyntaxNode()->children[0]);
        return child_evalutor->Evaluate(child_value_);
    }

    const std::shared_ptr<Value>& GetChildValue() const {
        return child_value_;
    }

private:
    std::shared_ptr<Value> child_value_;
};


class BinaryOperatorEvaluator : public OperatorEvaluator {
public:
    EvaluateStatus EvaluateChildren() override {

        auto child_evalutor = CreateValueEvaluator(GetSyntaxNode()->children[0]);
        auto status = child_evalutor->Evaluate(first_child_value_);
        if (status != EvaluateStatus::Ok) {
            return status;
        }

        child_evalutor = CreateValueEvaluator(GetSyntaxNode()->children[1]);
        return child_evalutor->Evaluate(second_child_value_);
    }

    const std::shared_ptr<Value>& GetFirstChildValue() const {
        return first_child_value_;
    }

    const std::shared_ptr<Value>& GetSecondChildValue() const {
        return second_child_value_;
    }

private:
    std::shared_ptr<Value> first_child_value_;
    std::shared_ptr<Value> second_child_value_;
};


class PositiveOperatorEvaluator : public UnaryOperatorEvaluator {
public:
    EvaluateStatus EvaluateValue(std::shared_ptr<Value>& value) override {
        value = GetChildValue();
        return EvaluateStatus::Ok;
    }
};


class NegativeOperatorEvaluator : public UnaryOperatorEvaluator {
public:
    EvaluateStatus EvaluateValue(std::shared_ptr<Value>& value) override {
        value = GetChildValue();
        value->integer_value *= -1;
        return EvaluateStatus::Ok;
    }
};


class NotOperatorEvaluator : public UnaryOperatorEvaluator {
public:
    EvaluateStatus EvaluateValue(std::shared_ptr<Value>& value) override {
        value = GetChildValue();
        value->integer_value = ~value->integer_value;
        return EvaluateStatus::Ok;
    }
};


class PlusOperatorEvaluator : public BinaryOperatorEvaluator {
public:
    EvaluateStatus EvaluateValue(std::shared_ptr<Value>& value) override {
        value = GetFirstChildValue();
        value->integer_value += GetSecondChildValue()->integer_value;
        return EvaluateStatus::Ok;
    }
};


class MinusOperatorEvaluator : public BinaryOperatorEvaluator {
public:
    EvaluateStatus EvaluateValue(std::shared_ptr<Value>& value) override {
        value = GetFirstChildValue();
        value->integer_value -= GetSecondChildValue()->integer_value;
        return EvaluateStatus::Ok;
    }
};


class MultiplyOperatorEvaluator : public BinaryOperatorEvaluator {
public:
    EvaluateStatus EvaluateValue(std::shared_ptr<Value>& value) override {
        value = GetFirstChildValue();
        value->integer_value *= GetSecondChildValue()->integer_value;
        return EvaluateStatus::Ok;
    }
};


class DivideOperatorEvaluator : public BinaryOperatorEvaluator {
public:
    EvaluateStatus EvaluateValue(std::shared_ptr<Value>& value) override {
        value = GetFirstChildValue();
        value->integer_value /= GetSecondChildValue()->integer_value;
        return EvaluateStatus::Ok;
    }
};


std::shared_ptr<OperatorEvaluator> CreateOperatorEvaluator(const std::shared_ptr<OperatorNode>& operator_node) {

    switch (operator_node->type) {

        case OperatorNode::Type::Positive:
            return std::make_shared<PositiveOperatorEvaluator>();

        case OperatorNode::Type::Negative:
            return std::make_shared<NegativeOperatorEvaluator>();

        case OperatorNode::Type::Not:
            return std::make_shared<NotOperatorEvaluator>();

        case OperatorNode::Type::Plus:
            return std::make_shared<PlusOperatorEvaluator>();

        case OperatorNode::Type::Minus:
            return std::make_shared<MinusOperatorEvaluator>();

        case OperatorNode::Type::Multiply:
            return std::make_shared<MultiplyOperatorEvaluator>();

        case OperatorNode::Type::Divide:
            return std::make_shared<DivideOperatorEvaluator>();

        default:
            return nullptr;
    }
}