#pragma once

#include <type_traits>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/base/error/error.h>
#include <zaf/creation.h>
#include <zaf/object/object.h>

namespace zaf::internal {

template<typename T>
class DefaultInstanceCreator {
public:
    static std::shared_ptr<T> Create() {
        return zaf::Create<T>();
    }
};

class UnsupportedInstanceCreator {
public:
    static std::shared_ptr<Object> Create() {
        throw InvalidOperationError{ ZAF_SOURCE_SITE() };
    }
};

template<typename T>
class InstanceCreatorSelector {
public:
    using Type = std::conditional_t<
        std::is_default_constructible_v<T>,
        DefaultInstanceCreator<T>,
        UnsupportedInstanceCreator
    >;
};

}