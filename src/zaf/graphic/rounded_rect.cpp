#include <zaf/graphic/rounded_rect.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>

namespace zaf {

void RoundedRect::SerializeToDataNode(DataNode& data_node) const {

    data_node.AddChild(property::XRadius, DataNode::CreateNumber(x_radius));
    data_node.AddChild(property::YRadius, DataNode::CreateNumber(y_radius));
    data_node.AddChild(property::Rect, rect.Serialize());
}


bool RoundedRect::DeserializeFromDataNode(const DataNode& data_node) {

    data_node.EnumerateKeyedChildren([this](const std::wstring& key, const DataNode& data_node) {
    
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


ZAF_DEFINE_TYPE_NAME(RoundedRect);

}