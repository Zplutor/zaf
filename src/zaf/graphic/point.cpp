#include <zaf/graphic/point.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>
#include <zaf/serialization/types.h>

namespace zaf {

std::wstring Point::GetTypeName() const {
    return type::Point;
}


void Point::SerializeToDataNode(DataNode& data_node) const {

    data_node.AddChild(property::X, DataNode::CreateNumber(x));
    data_node.AddChild(property::Y, DataNode::CreateNumber(y));
}


bool Point::DeserializeFromDataNode(const DataNode& data_node) {

    data_node.EnumerateKeyedChildren([this](const std::wstring& key, const DataNode& data_node) {
    
        if (key == property::X) {
            x = data_node.GetFloat();
        }
        else if (key == property::Y) {
            y = data_node.GetFloat();
        }
    });

    return true;
}

}