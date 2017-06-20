#include <zaf/graphic/frame.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>
#include <zaf/serialization/types.h>

namespace zaf {

std::wstring Frame::GetTypeName() const {
    return type::Frame;
}


void Frame::SerializeToDataNode(DataNode& data_node) const {

    data_node.AddChild(property::Left, DataNode::CreateNumber(left));
    data_node.AddChild(property::Top, DataNode::CreateNumber(top));
    data_node.AddChild(property::Right, DataNode::CreateNumber(right));
    data_node.AddChild(property::Bottom, DataNode::CreateNumber(bottom));
}


bool Frame::DeserializeFromDataNode(const DataNode& data_node) {

    data_node.EnumerateKeyedChildren([this](const std::wstring& key, const DataNode& data_node) {
    
        if (key == property::Left) {
            left = data_node.GetFloat();
        }
        else if (key == property::Top) {
            top = data_node.GetFloat();
        }
        else if (key == property::Right) {
            right = data_node.GetFloat();
        }
        else if (key == property::Bottom) {
            bottom = data_node.GetFloat();
        }
    });

    return true;
}

}