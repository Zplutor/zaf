#include <zaf/graphic/ellipse.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>

namespace zaf {

void Ellipse::SerializeToDataNode(DataNode& data_node) const {
    
    data_node.AddChild(property::Position, position.Serialize());
    data_node.AddChild(property::XRadius, DataNode::CreateNumber(x_radius));
    data_node.AddChild(property::YRadius, DataNode::CreateNumber(y_radius));
}


bool Ellipse::DeserializeFromDataNode(const DataNode& data_node) {

    data_node.EnumerateKeyedChildren([this](const std::wstring& key, const DataNode& data_node) {
    
        if (key == property::Position) {
            position.Deserialize(data_node);
        }
        else if (key == property::XRadius) {
            x_radius = data_node.GetFloat();
        }
        else if (key == property::YRadius) {
            y_radius = data_node.GetFloat();
        }
    });

    return true;
}


ZAF_DEFINE_TYPE_NAME(Ellipse);

}