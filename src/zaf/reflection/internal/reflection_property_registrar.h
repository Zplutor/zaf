#pragma once

#include <zaf/reflection/reflection_type.h>

namespace zaf::internal {

class ReflectionPropertyRegistrar {
public:
	static void Register(ReflectionType* type, ReflectionProperty* property) {
		type->RegisterProperty(property);
	}

public:
	ReflectionPropertyRegistrar() = delete;
};

}