#pragma once

#include "logic/syntax_tree/syntax_node.h"

class LengthNode : public SyntaxNode {
public:
    enum class Type {
        None,
        Byte,
        Word,
        DoubleWord,
        QuadrupleWord,
    };

    Type type = Type::None;
};