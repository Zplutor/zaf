#include <zaf/object/object.h>
#include <zaf/object/object_type.h>
#include <sstream>
#include <zaf/application.h>
#include <zaf/base/error/system_error.h>
#include <zaf/object/parsing/internal/default_object_parser.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_reader.h>
#include <zaf/base/string/encoding_conversion.h>
#include <zaf/object/internal/reflection_manager.h>
#include <zaf/resource/resource_manager.h>

namespace zaf {
namespace {

class TheType : public ObjectType {
public:
    ObjectType* GetBase() const override {
        return nullptr;
    }

    const std::wstring& GetName() const override {
        static const std::wstring name{ L"Object" };
        return name;
    }

    std::shared_ptr<Object> CreateInstance() const override {
        return Object::Dumb();
    }

protected:
    ObjectParser* GetSelfParser() const override {
        static internal::DefaultObjectParser parser;
        return &parser;
    }
};


void ParseObject(ObjectType& type, Object& object) {

    auto base_type = type.GetBase();
    if (base_type) {
        ParseObject(*base_type, object);
    }

    const auto& resource_uri = type.GetResourceURI();
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

const std::shared_ptr<Object>& Object::Dumb() {

    static auto empty_object = std::make_shared<Object>();
    return empty_object;
}


void Object::InitializeObject() {

    InvokeInitialize();
    ParseObject(*GetType(), *this);
    AfterParse();
}


void Object::InvokeInitialize() {
    Initialize();
}


void Object::Initialize() {

}


void Object::AfterParse() {

}


bool Object::IsEqual(const Object& other) const {

    if (typeid(*this) == typeid(other)) {
        return this == &other;
    }

    return false;
}


std::size_t Object::Hash() const {
    return std::hash<std::uintptr_t>()(reinterpret_cast<std::uintptr_t>(this));
}


std::wstring Object::ToString() const {

    std::ostringstream stream;
    stream << this << ' ' << typeid(*this).name();

    return FromUtf8String(stream.str());
}


ObjectType* const Object::Type = []() {

    static TheType type;
    zaf::internal::ReflectionManager::Instance().RegisterType(&type);
    return &type;
}();

}