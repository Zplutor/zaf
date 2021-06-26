#include <zaf/reflection/reflection_object.h>
#include <zaf/application.h>
#include <zaf/base/error/system_error.h>
#include <zaf/parsing/parser.h>
#include <zaf/parsing/xaml_reader.h>
#include <zaf/reflection/reflection_manager.h>
#include <zaf/reflection/reflection_type.h>
#include <zaf/resource/resource_manager.h>

namespace zaf {
namespace {

class ReflectionObjectType : public ObjectType {
public:
    ObjectType* GetBase() const override {
        return nullptr;
    }

    const std::wstring& GetName() const override {
        static const std::wstring name{ L"ReflectionObject" };
        return name;
    }

    std::shared_ptr<ReflectionObject> CreateInstance() const override {
        return std::make_shared<ReflectionObject>();
    }
};


void ParseObject(ObjectType& type, ReflectionObject& object) {

    auto base_type = type.GetBase();
    if (base_type) {
        ParseObject(*base_type, object);
    }

    auto resource_uri = type.GetResourceUri();
    if (resource_uri.empty()) {
        return;
    }

    auto stream = GetResourceManager().LoadUri(resource_uri);
    auto xaml_reader = XamlReader::FromStream(stream);

    auto root_node = xaml_reader->Read();
    if (root_node->GetValue() != type.GetName()) {
        ZAF_THROW_SYSTEM_ERROR(ERROR_INVALID_NAME);
    }

    type.GetParser()->ParseFromNode(*root_node, object);
}

}


void ReflectionObject::InitializeObject() {

    InvokeInitialize();
    ParseObject(*GetType(), *this);
    AfterParse();
}


void ReflectionObject::InvokeInitialize() {
    Initialize();
}


ObjectType* const ReflectionObject::Type = []() {

    static ReflectionObjectType type;
    zaf::GetReflectionManager().RegisterType(&type);
    return &type;
}();

}