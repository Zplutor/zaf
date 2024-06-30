#pragma once

#include <memory>
#include <string>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/non_copyable.h>

namespace zaf {

class Object;
class ObjectType;

class ObjectProperty : NonCopyableNonMovable {
public:
    ObjectProperty() = default;
    virtual ~ObjectProperty() = default;

    virtual const std::wstring& Name() const = 0;

    virtual bool CanGet() const = 0;
    virtual bool CanSet() const = 0;

    virtual bool IsValueDynamic() const = 0;

    virtual ObjectType* ValueType() const = 0;

    virtual std::shared_ptr<Object> GetValue(const Object& object) const = 0;
    virtual void SetValue(Object& object, const std::shared_ptr<Object>& value) const = 0;
};

}