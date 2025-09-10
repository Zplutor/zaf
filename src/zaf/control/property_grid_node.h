#pragma once

#include <vector>
#include <zaf/dynamic/object_property.h>

namespace zaf {

class PropertyGridNode {
public:
    explicit PropertyGridNode(dynamic::ObjectProperty* property) : property_(property) { }

    dynamic::ObjectProperty* Property() const {
        return property_;
    }

    const std::vector<PropertyGridNode>& Children() const {
        return children_;
    }

    PropertyGridNode* AddChild(dynamic::ObjectProperty* property) {
        return &children_.emplace_back(property);
    }

private:
    dynamic::ObjectProperty* property_{};
    std::vector<PropertyGridNode> children_;
};

}