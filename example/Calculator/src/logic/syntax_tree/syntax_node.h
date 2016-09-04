#pragma once

#include <memory>
#include <vector>

class SyntaxNode {
public:
    virtual ~SyntaxNode() { }
    std::vector<std::shared_ptr<SyntaxNode>> children;
};
