#pragma once

#include <memory>

namespace zaf {

class ReflectionType;

class ReflectionObject {
public:
    virtual ~ReflectionObject() = default;

    virtual std::shared_ptr<ReflectionType> GetType() const = 0;
};


#define ZAF_DECLARE_REFLECTION_TYPE()                       \
static const std::shared_ptr<ReflectionType> Type;          \
std::shared_ptr<ReflectionType> GetType() const override {  \
    return this->Type;                                      \
}

}