#pragma once

#include <memory>
#include <string>
#include <zaf/object/object_support.h>
#include <zaf/object/object_type.h>

namespace zaf {
namespace internal {
class ObjectInitializer;
}

class Object {
public:
    class Type : public ObjectType {
    public:
        static Type* Instance();

    public:
        ObjectType* BaseType() const noexcept override;
        std::wstring_view Name() const noexcept override;
        std::shared_ptr<Object> CreateInstance() const override;
        ObjectParser* Parser() const;

    private:
        static Type instance;
        Type();
    };

public:
    static ObjectType* StaticType();

public:
    Object() = default;
    virtual ~Object() = default;

    virtual ObjectType* DynamicType() const;
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