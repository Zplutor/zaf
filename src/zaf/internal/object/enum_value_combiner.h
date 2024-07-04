#pragma once

#include <zaf/base/as.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/base/flags_enum.h>
#include <zaf/object/object.h>

namespace zaf::internal {

template<typename EnumT, typename ObjectT, typename = void>
struct EnumValueCombiner {

    static void Combine(Object& result, const Object& value) {
        throw zaf::InvalidOperationError{ ZAF_SOURCE_LOCATION() };
    }
};


template<typename EnumT, typename ObjectT>
struct EnumValueCombiner<EnumT, ObjectT, std::enable_if_t<IsFlagsEnumV<EnumT>>> {

    static void Combine(Object& result, const Object& value) {
        As<ObjectT>(result).Value() |= As<ObjectT>(value).Value();
    }
};

}
