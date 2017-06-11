#pragma once

#include <memory>
#include <string>

namespace zaf {

class DataNode;
class SerializableType;

class SerializableObject {
public:
    virtual ~SerializableObject() { }

    std::shared_ptr<SerializableType> GetType() const;
    std::shared_ptr<DataNode> Serialize() const;
    bool Deserialize(const DataNode& data_node);

protected:
    virtual std::wstring GetTypeName() const = 0;
    virtual void SerializeToDataNode(DataNode& data_node) const = 0;
    virtual bool DeserializeFromDataNode(const DataNode& data_node) = 0;

protected:
    SerializableObject() { }
    SerializableObject(const SerializableObject&) { }
    SerializableObject& operator=(const SerializableObject&) { return *this; }
};

}