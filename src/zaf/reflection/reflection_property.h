#pragma once

#include <any>
#include <string>

namespace zaf {

class ReflectionObject;
class ObjectType;

class ObjectProperty {
public:
	ObjectProperty() = default;
	virtual ~ObjectProperty() = default;

	ObjectProperty(const ObjectProperty&) = delete;
	ObjectProperty& operator=(const ObjectProperty&) = delete;

	virtual const std::wstring& GetName() const = 0;

	virtual ObjectType* GetValueType() const = 0;

	virtual std::any GetValue(const ReflectionObject& object) const = 0;
	virtual void SetValue(ReflectionObject& object, const std::any& value) const = 0;
};

}