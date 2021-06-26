#include <zaf/reflection/reflection_manager.h>
#include <zaf/reflection/reflection_object.h>
#include <zaf/reflection/reflection_type.h>

namespace zaf {

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