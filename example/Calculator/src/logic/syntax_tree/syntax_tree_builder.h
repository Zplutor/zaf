#pragma once 

#include <memory>
#include <vector>

class OperandNode;
class OperatorNode;
class SyntaxNode;

class SyntaxTreeBuilder {
public:
    void AddOperator(const std::shared_ptr<OperatorNode>& operator_node);
    void AddOperand(const std::shared_ptr<SyntaxNode>& operand_node);

    std::shared_ptr<SyntaxNode> GetRootNode();

private:
    void MergeTopOperator();

private:
    std::vector<std::shared_ptr<OperatorNode>> operator_stack_;
    std::vector<std::shared_ptr<SyntaxNode>> operand_stack_;
};