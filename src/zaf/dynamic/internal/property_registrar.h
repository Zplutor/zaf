#pragma once

#include <zaf/dynamic/object_type.h>

namespace zaf::internal {

class PropertyRegistrar {
public:
    static void Register(dynamic::ObjectType* type, dynamic::ObjectProperty* property) {
        type->RegisterProperty(property);
    }

public:
    PropertyRegistrar() = delete;
};

}