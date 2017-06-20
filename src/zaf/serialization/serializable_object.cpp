#include <zaf/serialization/serializable_object.h>
#include <zaf/application.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>
#include <zaf/serialization/serialization_manager.h>

namespace zaf {

std::shared_ptr<SerializableType> SerializableObject::GetType() const {
    return GetSerializationManager()->GetType(GetTypeName());
}


std::shared_ptr<DataNode> SerializableObject::Serialize() const {

    auto data_node = DataNode::CreateObject();
    data_node->AddChild(property::Type, DataNode::CreateString(GetTypeName()));

    SerializeToDataNode(*data_node);
    return data_node;
}


bool SerializableObject::Deserialize(const DataNode& data_node) {

    if (! data_node.IsObject()) {
        return false;
    }

    auto type_node = data_node.GetChild(property::Type);
    if (type_node != nullptr) {

        if (type_node->GetString() != GetTypeName()) {
            return false;
        }
    }

    return DeserializeFromDataNode(data_node);
}

}