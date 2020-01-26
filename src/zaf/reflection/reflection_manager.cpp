#include <zaf/reflection/reflection_manager.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/container/utility/map.h>
#include <zaf/reflection/reflection_object.h>
#include <zaf/reflection/reflection_type.h>

namespace zaf {

void ReflectionManager::RegisterType(const std::shared_ptr<ReflectionType>& type) {

    types_[type->GetName()] = type;
}


std::shared_ptr<ReflectionType> ReflectionManager::GetType(const std::wstring& name) const {

    auto type = Find(types_, name);
    if (type != nullptr) {
        return *type;
    }
    return nullptr;
}


void ReflectionManager::CheckTypeErrors() {
#ifndef NDEBUG

    for (const auto& each_type: Values(types_)) {

        auto base_type = each_type->GetBase();
        if (!base_type) {
            continue;
        }

        auto instance = each_type->CreateInstance();
        if (instance->__zaf_internal_GetBaseClassName() != base_type->GetName()) {
            throw std::logic_error("Base type error.");
        }
    }

#endif
}

}