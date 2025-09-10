#pragma once

#include <memory>
#include <string>
#include <zaf/base/error/precondition_error.h>
#include <zaf/base/non_copyable.h>

namespace zaf {

class Object;
class ObjectType;

class ObjectProperty : NonCopyableNonMovable {
public:
    ObjectProperty() = default;
    virtual ~ObjectProperty() = default;

    virtual std::wstring_view Name() const noexcept = 0;

    virtual bool CanGet() const noexcept = 0;
    virtual bool CanSet() const noexcept = 0;

    virtual bool IsValueDynamic() const noexcept = 0;

    virtual ObjectType* ValueType() const noexcept = 0;

    virtual std::shared_ptr<Object> GetValue(const Object& object) const = 0;
    virtual void SetValue(Object& object, const std::shared_ptr<Object>& value) const = 0;
};

}