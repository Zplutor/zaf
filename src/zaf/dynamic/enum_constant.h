#pragma once

#include <string>
#include <zaf/base/non_copyable.h>
#include <zaf/dynamic/object.h>

namespace zaf {

class EnumConstant : NonCopyableNonMovable {
public:
    EnumConstant() = default;
    virtual ~EnumConstant() = default;

    virtual const std::wstring& Name() const = 0;
    
    virtual ObjectType* ValueType() const = 0;
    virtual std::shared_ptr<Object> Value() const = 0;
};

}