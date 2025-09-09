#pragma once

#include <vector>
#include <zaf/dynamic/object_property.h>

namespace zaf {

class PropertyGridNode {
public:
    explicit PropertyGridNode(ObjectProperty* property) : property_(property) { }

    ObjectProperty* Property() const {
        return property_;
    }

    const std::vector<PropertyGridNode>& Children() const {
        return children_;
    }

    PropertyGridNode* AddChild(ObjectProperty* property) {
        return &children_.emplace_back(property);
    }

private:
    ObjectProperty* property_{};
    std::vector<PropertyGridNode> children_;
};

}