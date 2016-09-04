#pragma once

#include "logic/syntax_tree/syntax_node.h"

class OperandNode : public SyntaxNode {
public:
    std::size_t radix = 0;
    std::wstring text;
};