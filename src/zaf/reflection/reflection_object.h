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
    static ReflectionType* const Type;

public:
    virtual ~ReflectionObject() = default;

    virtual ReflectionType* GetType() const {
        return this->Type;
    }

    ReflectionType* GetBaseType() const {
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
static zaf::ReflectionType* const Type;                                                          \
zaf::ReflectionType* GetType() const override {                                                  \
    return this->Type;                                                                           \
}                                                                                                \
zaf::ReflectionType* GetBaseType() const {                                                       \
    return __super::Type;                                                                        \
}                                                                                                

}