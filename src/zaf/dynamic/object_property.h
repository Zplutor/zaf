#pragma once

#include <memory>
#include <string>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/non_copyable.h>

namespace zaf::dynamic {
class Object;
class ObjectType;
}

namespace zaf {

class ObjectProperty : NonCopyableNonMovable {
public:
    ObjectProperty() = default;
    virtual ~ObjectProperty() = default;

    virtual std::wstring_view Name() const noexcept = 0;

    virtual bool CanGet() const noexcept = 0;
    virtual bool CanSet() const noexcept = 0;

    virtual bool IsValueDynamic() const noexcept = 0;

    virtual dynamic::ObjectType* ValueType() const noexcept = 0;

    virtual std::shared_ptr<dynamic::Object> GetValue(const dynamic::Object& object) const = 0;
    virtual void SetValue(dynamic::Object& object, const std::shared_ptr<dynamic::Object>& value) const = 0;
};

}