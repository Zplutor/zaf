#pragma once

#include <functional>

#define ZAF_DECLARE_EQUALITY_TYPE                                                                  \
bool IsEqual(const Object& other) const override;                                                  \
std::size_t Hash() const override;                                                                 


#define ZAF_DEFINE_EQUALITY_TYPE(ClassName)                                                        \
bool ClassName::IsEqual(const Object& other) const {                                               \
    auto other_object = dynamic_cast<const ClassName*>(&other);                                    \
    if (other_object) {                                                                            \
        return *this == *other_object;                                                             \
    }                                                                                              \
    return false;                                                                                  \
}                                                                                                  \
std::size_t ClassName::Hash() const {                                                              \
    return std::hash<ClassName>()(*this);                                                          \
}