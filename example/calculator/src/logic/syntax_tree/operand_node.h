#pragma once

#include "logic/syntax_tree/syntax_node.h"

class OperandNode : public SyntaxNode {
public:
    std::wstring text;
};