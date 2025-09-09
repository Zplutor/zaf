#pragma once

#include <memory>
#include <string>
#include <zaf/dynamic/object_support.h>
#include <zaf/dynamic/object_type.h>

namespace zaf::internal {
class ObjectInitializer;
}

namespace zaf::dynamic {

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

    virtual dynamic::ObjectType* DynamicType() const;
    virtual bool IsEqual(const dynamic::Object& other) const;
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