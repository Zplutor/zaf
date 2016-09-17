#pragma once

#include <memory>
#include "logic/evaluate/evaluate_status.h"

class SyntaxNode;
class Value;

class ValueEvaluator {
public:
    virtual EvaluateStatus Evaluate(std::shared_ptr<Value>& value) = 0;
};

template<typename SyntaxNodeType>
class BaseValueEvaluator : public ValueEvaluator {
public:
    const std::shared_ptr<SyntaxNodeType>& GetSyntaxNode() const {
        return syntax_node_;
    }

    void SetSyntaxNode(const std::shared_ptr<SyntaxNodeType>& node) {
        syntax_node_ = node;
    }

private:
    std::shared_ptr<SyntaxNodeType> syntax_node_;
};

std::shared_ptr<ValueEvaluator> CreateValueEvaluator(const std::shared_ptr<SyntaxNode>& syntax_node);