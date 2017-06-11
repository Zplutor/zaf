#include <zaf/graphic/ellipse.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>
#include <zaf/serialization/types.h>

namespace zaf {

std::wstring Ellipse::GetTypeName() const {
    return type::Ellipse;
}


void Ellipse::SerializeToDataNode(DataNode& data_node) const {
    
    data_node.AddField(property::Position, position.Serialize());
    data_node.AddField(property::XRadius, DataNode::CreateNumber(x_radius));
    data_node.AddField(property::YRadius, DataNode::CreateNumber(y_radius));
}


bool Ellipse::DeserializeFromDataNode(const DataNode& data_node) {

    data_node.EnumerateFields([this](const std::wstring& key, const DataNode& data_node) {
    
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

}