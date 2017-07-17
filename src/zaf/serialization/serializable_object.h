#pragma once

#include <memory>
#include <string>

namespace zaf {

class DataNode;

class SerializableObject {
public:
    virtual ~SerializableObject() { }

    virtual const wchar_t* GetTypeName() const = 0;

    std::shared_ptr<DataNode> Serialize() const;
    bool Deserialize(const DataNode& data_node);

protected:
    virtual void SerializeToDataNode(DataNode& data_node) const = 0;
    virtual bool DeserializeFromDataNode(const DataNode& data_node) = 0;

    SerializableObject() { }
    SerializableObject(const SerializableObject&) { }
    SerializableObject& operator=(const SerializableObject&) { return *this; }
};


#define ZAF_DECLARE_TYPE_NAME()                \
static const wchar_t TypeName[];               \
const wchar_t* GetTypeName() const override {  \
    return TypeName;                           \
}

#define ZAF_DEFINE_TYPE_NAME(Type)       \
const wchar_t Type::TypeName[] = L#Type; 

#define ZAF_DEFINE_SCOPED_TYPE_NAME(Type, Scope)      \
const wchar_t Type::TypeName[] = L#Type L"." L#Scope;

#define ZAF_DEFINE_INNER_TYPE_NAME(OuterType, InnerType)                       \
const wchar_t OuterType::InnerType::TypeName[] = L#InnerType L"." L#OuterType; 

#define ZAF_DEFINE_SCOPED_INNER_TYPE_NAME(OuterType, InnerType, Scope)                      \
const wchar_t OuterType::InnerType::TypeName[] = L#InnerType L"." L#OuterType L"." L#Scope; 

}