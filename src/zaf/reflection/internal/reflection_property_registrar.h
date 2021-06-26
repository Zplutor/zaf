#pragma once

#include <zaf/reflection/reflection_type.h>

namespace zaf::internal {

class ReflectionPropertyRegistrar {
public:
	static void Register(ObjectType* type, ObjectProperty* property) {
		type->RegisterProperty(property);
	}

public:
	ReflectionPropertyRegistrar() = delete;
};

}