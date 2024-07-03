#include <zaf/object/enum_type.h>

namespace zaf {

void EnumType::RegisterConstant(EnumConstant* constant) {

    auto iterator = std::lower_bound(
        constants_.begin(),
        constants_.end(),
        constant,
        [](auto constant1, auto constant2) {

        return constant1->Name() < constant2->Name();
    });

    constants_.insert(iterator, constant);
}


EnumConstant* EnumType::GetConstant(std::wstring_view name) const noexcept {

    auto iterator = std::lower_bound(
        constants_.begin(),
        constants_.end(),
        name,
        [](auto constant, std::wstring_view name) {

        return constant->Name() < name;
    });

    if (iterator != constants_.end() && (*iterator)->Name() == name) {
        return *iterator;
    }

    return nullptr;
}


std::vector<std::shared_ptr<Object>> EnumType::Values() const {

    std::vector<std::shared_ptr<Object>> result;
    for (auto each_constant : Constants()) {
        result.push_back(each_constant->Value());
    }
    return result;
}


std::shared_ptr<Object> EnumType::GetValue(std::wstring_view name) const noexcept {

    auto constant = GetConstant(name);
    if (constant) {
        return constant->Value();
    }
    return nullptr;
}


std::wstring EnumType::GetValueName(const Object& object) const {

    for (auto each_constant : Constants()) {
        if (each_constant->Value()->IsEqual(object)) {
            return each_constant->Name();
        }
    }
    return std::wstring{};
}

}