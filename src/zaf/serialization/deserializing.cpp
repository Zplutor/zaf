#include <zaf/serialization/deserializing.h>
#include <zaf/application.h>
#include <zaf/serialization/properties.h>
#include <zaf/serialization/serialization_manager.h>

namespace zaf {
namespace internal {

std::shared_ptr<SerializableObject> DeserializeObject(const DataNode& data_node) {

    auto type_node = data_node.GetChild(property::Type);
    if (type_node == nullptr) {
        return nullptr;
    }

    auto object = GetSerializationManager()->CreateObject(type_node->GetString());
    if (! object->Deserialize(data_node)) {
        return nullptr;
    }

    return object;
}

}
}