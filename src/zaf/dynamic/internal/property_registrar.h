#pragma once

#include <zaf/dynamic/object_type.h>

namespace zaf::internal {

class PropertyRegistrar {
public:
    static void Register(ObjectType* type, ObjectProperty* property) {
        type->RegisterProperty(property);
    }

public:
    PropertyRegistrar() = delete;
};

}