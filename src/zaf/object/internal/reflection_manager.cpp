#include <zaf/object/internal/reflection_manager.h>
#include <zaf/object/object.h>
#include <zaf/object/object_type.h>

namespace zaf::internal {

ReflectionManager& ReflectionManager::Instance() {
    static ReflectionManager instance;
    return instance;
}


void ReflectionManager::RegisterType(ObjectType* type) {

    auto iterator = std::lower_bound(
        types_.begin(), 
        types_.end(),
        type, 
        [](auto type1, auto type2) {
    
        return type1->GetName() < type2->GetName();
    });

    types_.insert(iterator, type);
}


ObjectType* ReflectionManager::GetType(const std::wstring& name) const {

    auto iterator = std::lower_bound(
        types_.begin(),
        types_.end(),
        name,
        [](auto type, const std::wstring& name) {

        return type->GetName() < name;
    });

    if (iterator == types_.end()) {
        return nullptr;
    }

    if ((*iterator)->GetName() != name) {
        return nullptr;
    }

    return *iterator;
}

}