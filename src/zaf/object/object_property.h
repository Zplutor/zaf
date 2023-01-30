#pragma once

#include <memory>
#include <string>
#include <zaf/base/error/check.h>

namespace zaf {

class Object;
class ObjectType;

class ObjectProperty {
public:
    ObjectProperty() = default;
    virtual ~ObjectProperty() = default;

    ObjectProperty(const ObjectProperty&) = delete;
    ObjectProperty& operator=(const ObjectProperty&) = delete;

    virtual const std::wstring& GetName() const = 0;

    virtual bool CanGet() const = 0;
    virtual bool CanSet() const = 0;

    virtual bool IsValueTypeDynamic() const = 0;

    virtual ObjectType* GetValueType() const = 0;

    virtual std::shared_ptr<Object> GetValue(const Object& object) const = 0;
    virtual void SetValue(Object& object, const std::shared_ptr<Object>& value) const = 0;
};

}