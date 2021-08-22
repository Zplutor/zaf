#pragma once

#include <string>
#include <zaf/object/type_declaration.h>

namespace zaf {
namespace internal {
class ObjectInitializer;
}

class ObjectType;

class Object {
public:
    static ObjectType* const Type;
    static const std::shared_ptr<Object>& Dumb();

public:
    Object() = default;
    virtual ~Object() = default;

    virtual ObjectType* GetType() const {
        return Object::Type;
    }

    ObjectType* GetBaseType() const {
        return nullptr;
    }

    virtual bool IsEqual(const Object& other) const;
    virtual std::size_t Hash() const;

    virtual std::wstring ToString() const;

protected:
    virtual void InvokeInitialize();
    virtual void Initialize();
    virtual void InvokeParse();
    virtual void AfterParse();

private:
    friend class internal::ObjectInitializer;

    void InitializeObject();
};

}