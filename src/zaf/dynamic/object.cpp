#include <zaf/dynamic/object.h>
#include <zaf/dynamic/object_type.h>
#include <sstream>
#include <zaf/base/error/win32_error.h>
#include <zaf/creation.h>
#include <zaf/dynamic/parsing/object_parser.h>
#include <zaf/dynamic/parsing/xaml_reader.h>
#include <zaf/base/string/encoding_conversion.h>
#include <zaf/dynamic/internal/reflection_manager.h>
#include <zaf/resource/resource_factory.h>

namespace zaf::dynamic {
namespace {

void ParseObject(ObjectType& type, Object& object) {

    auto base_type = type.BaseType();
    if (base_type) {
        ParseObject(*base_type, object);
    }

    auto resource_uri = type.ResourceURI();
    if (resource_uri.empty()) {
        return;
    }

    auto stream = ResourceFactory::Instance().LoadURI(resource_uri);
    auto xaml_reader = XamlReader::FromStream(stream);

    auto root_node = xaml_reader->Read();
    if (root_node->Value() != type.Name()) {
        ZAF_THROW_WIN32_ERROR(ERROR_INVALID_NAME);
    }

    type.Parser()->ParseFromNode(*root_node, object);
}

}


Object::Type Object::Type::instance;

Object::Type* Object::Type::Instance() {
    return &instance;
}

Object::Type::Type() {
    zaf::internal::ReflectionManager::Instance().RegisterType(this);
}

ObjectType* Object::Type::BaseType() const noexcept {
    return nullptr;
}

std::wstring_view Object::Type::Name() const noexcept {
    return std::wstring_view{ L"Object" };
}

std::shared_ptr<Object> Object::Type::CreateInstance() const {
    return Create<dynamic::Object>();
}

ObjectParser* Object::Type::Parser() const {
    static ObjectParser default_parser;
    return &default_parser;
}


ObjectType* Object::StaticType() {
    return Type::Instance();
}


void Object::InitializeObject() {

    InvokeInitialize();
    InvokeParse();
    AfterParse();
}


void Object::InvokeInitialize() {
    Initialize();
}


void Object::Initialize() {

}


void Object::InvokeParse() {
    ParseObject(*DynamicType(), *this);
}


void Object::AfterParse() {

}


ObjectType* Object::DynamicType() const {
    return StaticType();
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

    return FromUTF8String(stream.str());
}

}