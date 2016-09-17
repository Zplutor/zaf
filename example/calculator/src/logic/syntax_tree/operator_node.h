#pragma once

#include "logic/syntax_tree/syntax_node.h"

class OperatorNode : public SyntaxNode {
public:
    enum class Type {

        None,

        LeftParenthesis,
        RightParenthesis,

        Positive,
        Negative,

        Plus,
        Minus,
        Multiply,
        Divide,
    };

    int GetPriority() const {

        switch (type) {

            case Type::Positive:
            case Type::Negative:
                return 102;

            case Type::Multiply:
            case Type::Divide:
                return 101;

            case Type::Plus:
            case Type::Minus:
                return 100;

            default:
                return 0;
        }
    }

    Type type = Type::None;
};