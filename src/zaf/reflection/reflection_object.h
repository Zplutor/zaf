#pragma once

#include <memory>
#include <zaf/reflection/reflection_type.h>

namespace zaf {
namespace internal {

class ReflectionObjectInitializer;

}

class ReflectionObject {
public:
    static const std::shared_ptr<ReflectionType> Type;

public:
    virtual ~ReflectionObject() = default;

    virtual std::shared_ptr<ReflectionType> GetType() const {
        return this->Type;
    }

    std::shared_ptr<ReflectionType> GetBaseType() const {
        return {};
    }

protected:
    virtual void Initialize() { }

private:
    friend class internal::ReflectionObjectInitializer;

    void InitializeObject();
};


#define ZAF_DECLARE_REFLECTION_TYPE()                                                            \
static const std::shared_ptr<zaf::ReflectionType> Type;                                          \
std::shared_ptr<zaf::ReflectionType> GetType() const override {                                  \
    return this->Type;                                                                           \
}                                                                                                \
std::shared_ptr<zaf::ReflectionType> GetBaseType() const {                                       \
    return __super::Type;                                                                        \
}                                                                                                

}