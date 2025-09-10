#pragma once

#include <zaf/dynamic/enum_type.h>

namespace zaf::internal {

class EnumConstantRegistrar {
public:
    static void Register(dynamic::EnumType* type, EnumConstant* constant) {
        type->RegisterConstant(constant);
    }

public:
    EnumConstantRegistrar() = delete;
};

}