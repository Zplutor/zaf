#include <zaf/reflection/reflection_object.h>
#include <zaf/application.h>
#include <zaf/base/error/system_error.h>
#include <zaf/parsing/parser.h>
#include <zaf/parsing/xaml_reader.h>
#include <zaf/reflection/reflection_type.h>
#include <zaf/resource/resource_manager.h>

namespace zaf {
namespace {

class ReflectionObjectType : public ReflectionType {
public:
    std::shared_ptr<ReflectionType> GetBase() override {
        return {};
    }

    std::wstring GetName() override {
        return L"ReflectionObject";
    }

    std::shared_ptr<ReflectionObject> CreateInstance() override {
        return std::make_shared<ReflectionObject>();
    }
};


void InitializeReflectionObject(ReflectionType& type, ReflectionObject& object) {

    auto base_type = type.GetBase();
    if (base_type) {
        InitializeReflectionObject(*base_type, object);
    }

    auto resource_uri = type.GetResourceUri();
    if (resource_uri.empty()) {
        return;
    }

    auto stream = GetResourceManager().LoadUri(resource_uri);
    auto xaml_reader = XamlReader::CreateFromStream(stream);

    auto root_node = xaml_reader->Read();
    if (root_node->GetValue() != type.GetName()) {
        ZAF_THROW_SYSTEM_ERROR(ERROR_INVALID_NAME);
    }

    type.GetParser()->ParseFromNode(*root_node, object);
}

}


void ReflectionObject::Initialize() {
    InitializeReflectionObject(*GetType(), *this);
}


const std::shared_ptr<ReflectionType> ReflectionObject::Type = 
    std::make_shared<ReflectionObjectType>();

}