#pragma once

#include <zaf/object/object_type.h>

namespace zaf {

class EnumType : public ObjectType {
public:
    ObjectType* BaseType() const override {
        return nullptr;
    }

    virtual std::vector<std::shared_ptr<Object>> GetAllValues() const = 0;
    virtual std::shared_ptr<Object> FindValue(std::wstring_view name) const = 0;
    virtual std::wstring GetValueName(const Object& object) const = 0;
};

}