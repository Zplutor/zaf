#include <zaf/reflection/reflection_manager.h>
#include <zaf/base/container/utility/find.h>
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

}