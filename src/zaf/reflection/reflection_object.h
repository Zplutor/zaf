#pragma once

#include <memory>
#include <zaf/reflection/reflection_type.h>

namespace zaf {

class ReflectionObject {
public:
    static const std::shared_ptr<ReflectionType> Type;

public:
    virtual ~ReflectionObject() = default;

    virtual std::shared_ptr<ReflectionType> GetType() const {
        return this->Type;
    }

    virtual std::wstring __zaf_internal_GetBaseClassName() const {
        return {};
    }
};


#define ZAF_DECLARE_REFLECTION_TYPE()                               \
static const std::shared_ptr<ReflectionType> Type;                  \
std::shared_ptr<ReflectionType> GetType() const override {          \
    return this->Type;                                              \
}                                                                   \
std::wstring __zaf_internal_GetBaseClassName() const override {     \
    return __super::Type->GetName();                                \
}

}