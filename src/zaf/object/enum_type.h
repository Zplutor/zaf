#pragma once

#include <zaf/object/enum_constant.h>
#include <zaf/object/object_map.h>
#include <zaf/object/object_type.h>

namespace zaf {
namespace internal {
class EnumConstantRegistrar;
}

class EnumType : public ObjectType {
public:
    const std::vector<EnumConstant*>& Constants() const noexcept {
        return constants_;
    }

    EnumConstant* GetConstant(std::wstring_view name) const noexcept;

    std::vector<std::shared_ptr<Object>> Values() const;
    std::shared_ptr<Object> GetValue(std::wstring_view name) const noexcept;
    std::wstring GetValueName(const Object& object) const;

private:
    friend class internal::EnumConstantRegistrar;

    void RegisterConstant(EnumConstant* constant);

private:
    std::vector<EnumConstant*> constants_;
    ObjectMap<EnumConstant*> value_map_;
};

}