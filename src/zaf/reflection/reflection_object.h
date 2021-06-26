#pragma once

#include <memory>
#include <zaf/object/object.h>
#include <zaf/reflection/reflection_type.h>

namespace zaf {
namespace internal {
class ReflectionObjectInitializer;
}

class ReflectionObject : public Object {
public:
    static ObjectType* const Type;

public:
    virtual ~ReflectionObject() = default;

    virtual ObjectType* GetType() const {
        return this->Type;
    }

    ObjectType* GetBaseType() const {
        return nullptr;
    }

protected:
    virtual void InvokeInitialize();
    virtual void Initialize() { }
    virtual void AfterParse() { }

private:
    friend class internal::ReflectionObjectInitializer;

    void InitializeObject();
};


#define ZAF_DECLARE_REFLECTION_TYPE                                                              \
static zaf::ObjectType* const Type;                                                          \
zaf::ObjectType* GetType() const override {                                                  \
    return this->Type;                                                                           \
}                                                                                                \
zaf::ObjectType* GetBaseType() const {                                                       \
    return __super::Type;                                                                        \
}                                                                                                

}