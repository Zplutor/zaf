#include <zaf/serialization/deserializing.h>
#include <zaf/application.h>
#include <zaf/serialization/data_reader.h>
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
    if (object == nullptr) {
        return nullptr;
    }

    if (! object->Deserialize(data_node)) {
        return nullptr;
    }

    return object;
}


std::shared_ptr<DataNode> ParseDataNode(bool is_xml, const void* string, std::size_t length) {

    std::shared_ptr<DataReader> data_reader;
    if (is_xml) {
        data_reader = CreateXmlReader();
    }
    else {
        data_reader = CreateJsonReader();
    }

    std::error_code error_code;
    return data_reader->Read(string, length, error_code);
}

}
}