#include <zaf/dynamic/enum_type.h>
#include <zaf/base/error/contract_error.h>

namespace zaf {

void EnumType::RegisterConstant(EnumConstant* constant) {

    auto iterator = std::lower_bound(
        constants_.begin(),
        constants_.end(),
        constant,
        [](auto constant1, auto constant2) {

        return constant1->Name() < constant2->Name();
    });

    ZAF_EXPECT(iterator == constants_.end() || (*iterator)->Name() != constant->Name());

    constants_.insert(iterator, constant);
    value_map_.try_emplace(constant->Value(), constant);
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
    for (const auto& each_pair : value_map_) {
        result.push_back(each_pair.first);
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

    auto iterator = value_map_.find(object);
    if (iterator != value_map_.end()) {
        return iterator->second->Name();
    }

    return std::wstring{};
}

}