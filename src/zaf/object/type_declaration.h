#pragma once

#define ZAF_DECLARE_TYPE                                                                        \
static zaf::ObjectType* const Type;                                                             \
zaf::ObjectType* GetType() const override {                                                     \
    return this->Type;                                                                          \
}                                                                                               \
zaf::ObjectType* GetBaseType() const {                                                          \
    return __super::Type;                                                                       \
}
