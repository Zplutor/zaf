#pragma once

#include <memory>
#include <string>
#include <zaf/object/object_support.h>
#include <zaf/object/object_type.h>
#include <zaf/object/type_declaration.h>

namespace zaf {
namespace internal {
class ObjectInitializer;
}

class Object {
public:
    class StaticType : public ObjectType {
    public:
        static StaticType* Instance();

    public:
        ObjectType* GetBase() const override;
        const std::wstring& GetName() const override;
        std::shared_ptr<Object> CreateInstance() const override;
        ObjectParser* GetParser() const;

    private:
        static StaticType instance;
        StaticType();
    };

public:
    static ObjectType* const Type;

public:
    Object() = default;
    virtual ~Object() = default;

    virtual ObjectType* GetType() const {
        return Object::Type;
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