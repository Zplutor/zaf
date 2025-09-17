#include <zaf/object/reflection.h>
#include <zaf/object/internal/reflection_manager.h>

namespace zaf {

ObjectType* Reflection::GetType(std::wstring_view type_name) noexcept {
    return internal::ReflectionManager::Instance().GetType(type_name);
}


std::shared_ptr<Object> Reflection::CreateObjectByName(std::wstring_view type_name) {
    auto type = GetType(type_name);
    if (type) {
        return type->CreateInstance();
    }
    return nullptr;
}

}