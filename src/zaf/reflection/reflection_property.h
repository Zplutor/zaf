#pragma once

#include <any>
#include <string>

namespace zaf {

class ReflectionObject;
class ObjectType;

class ReflectionProperty {
public:
	ReflectionProperty() = default;
	virtual ~ReflectionProperty() = default;

	ReflectionProperty(const ReflectionProperty&) = delete;
	ReflectionProperty& operator=(const ReflectionProperty&) = delete;

	virtual const std::wstring& GetName() const = 0;

	virtual ObjectType* GetType() const = 0;

	virtual std::any GetValue(const ReflectionObject& object) const = 0;
	virtual void SetValue(ReflectionObject& object, const std::any& value) const = 0;
};

}