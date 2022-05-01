#include <zaf/object/parsing/xaml_node.h>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/container/utility/sort.h>

namespace zaf {

bool XamlNode::IsPropertyNode() const {

    if (Type() != XamlNodeType::Element) {
        return false;
    }

    const auto& value = Value();
    return Contain(value, L'.');
}


std::shared_ptr<XamlAttribute> XamlNode::FindAttribute(const std::wstring& name) const {

    std::call_once(sorted_attributes_once_flag_, [this]() {

        sorted_attributes_ = attributes_;
        Sort(sorted_attributes_, [](const auto& attribute1, const auto& attribute2) {
            return attribute1->Name() < attribute2->Name();
        });
    });

    auto iterator = std::lower_bound(
        sorted_attributes_.begin(),
        sorted_attributes_.end(),
        name, 
        [](const auto& attribute, const std::wstring& name) {
            return attribute->Name() < name;
        }
    );

    if (iterator == sorted_attributes_.end() || (*iterator)->Name() != name) {
        return nullptr;
    }

    return *iterator;
}


std::shared_ptr<XamlNode> XamlNode::FindPropertyNode(const std::wstring& name) const {

    std::call_once(sorted_property_nodes_once_flag_, [this]() {
    
        sorted_property_nodes_ = property_nodes_;
        Sort(sorted_property_nodes_, [](const auto& node1, const auto& node2) {
            return node1->Value() < node2->Value();
        });
    });

    auto iterator = std::lower_bound(
        sorted_property_nodes_.begin(),
        sorted_property_nodes_.end(), 
        name,
        [](const auto& node, const std::wstring& name) {
            return node->Value() < name;
        }
    );

    if (iterator == sorted_property_nodes_.end() || (*iterator)->Value() != name) {
        return nullptr;
    }
    
    return *iterator;
}

}