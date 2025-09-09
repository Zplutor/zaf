#pragma once

#include <zaf/dynamic/enum_constant.h>
#include <zaf/dynamic/object_map.h>
#include <zaf/dynamic/object_type.h>

namespace zaf {
namespace internal {
class EnumConstantRegistrar;
}

class EnumType : public dynamic::ObjectType {
public:
    const std::vector<EnumConstant*>& Constants() const noexcept {
        return constants_;
    }

    EnumConstant* GetConstant(std::wstring_view name) const noexcept;

    std::vector<std::shared_ptr<Object>> Values() const;
    std::shared_ptr<Object> GetValue(std::wstring_view name) const noexcept;
    std::wstring GetValueName(const Object& object) const;

    virtual void SetValue(Object& result, const Object& value) const = 0;

    virtual bool IsFlagsEnum() const noexcept = 0;
    virtual void CombineFlagValue(Object& result, const Object& value) const = 0;

private:
    friend class internal::EnumConstantRegistrar;

    void RegisterConstant(EnumConstant* constant);

private:
    std::vector<EnumConstant*> constants_;
    ObjectMap<EnumConstant*> value_map_;
};

}