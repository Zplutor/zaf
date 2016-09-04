#pragma once

#include "logic/syntax_tree/syntax_node.h"

class FormatNode : public SyntaxNode {
public:
    enum class Type {
        None,
        Binary,
        Octal,
        Decimal,
        Hex,
    };

    Type type = Type::None;
};