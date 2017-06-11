#pragma once

#include <functional>
#include <memory>
#include <string>

namespace zaf {

class SerializableObject;

typedef std::function<std::shared_ptr<SerializableObject>()> SerializableObjectCreator;

class SerializableTypeProperties {
public:
    std::wstring name;
    SerializableObjectCreator instance_creator;
};


class SerializableType {
public:
    SerializableType() { }
    virtual ~SerializableType() { }

    virtual std::wstring GetName() = 0;
    virtual std::shared_ptr<SerializableObject> CreateInstance() = 0;

    SerializableType(const SerializableType&) = delete;
    SerializableType& operator=(const SerializableType&) = delete;
};

}