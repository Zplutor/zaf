#include <zaf/graphic/rounded_rect.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>
#include <zaf/serialization/types.h>

namespace zaf {

std::wstring RoundedRect::GetTypeName() const {
    return type::RoundedRect;
}


void RoundedRect::SerializeToDataNode(DataNode& data_node) const {

    data_node.AddField(property::XRadius, DataNode::CreateNumber(x_radius));
    data_node.AddField(property::YRadius, DataNode::CreateNumber(y_radius));
    data_node.AddField(property::Rect, rect.Serialize());
}


bool RoundedRect::DeserializeFromDataNode(const DataNode& data_node) {

    data_node.EnumerateFields([this](const std::wstring& key, const DataNode& data_node) {
    
        if (key == property::XRadius) {
            x_radius = data_node.GetFloat();
        }
        else if (key == property::YRadius) {
            y_radius = data_node.GetFloat();
        }
        else if (key == property::Rect) {
            rect.Deserialize(data_node);
        }
    });

    return true;
}

}