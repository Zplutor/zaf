#include <zaf/object/object.h>
#include <zaf/object/object_type.h>
#include <sstream>
#include <zaf/base/error/win32_error.h>
#include <zaf/creation.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_reader.h>
#include <zaf/base/string/encoding_conversion.h>
#include <zaf/object/internal/reflection_manager.h>
#include <zaf/resource/resource_factory.h>

namespace zaf {
namespace {

void ParseObject(ObjectType& type, Object& object) {

    auto base_type = type.GetBase();
    if (base_type) {
        ParseObject(*base_type, object);
    }

    const auto& resource_uri = type.GetResourceURI();
    if (resource_uri.empty()) {
        return;
    }

    auto stream = ResourceFactory::Instance().LoadURI(resource_uri);
    auto xaml_reader = XamlReader::FromStream(stream);

    auto root_node = xaml_reader->Read();
    if (root_node->Value() != type.GetName()) {
        ZAF_THROW_WIN32_ERROR(ERROR_INVALID_NAME);
    }

    type.GetParser()->ParseFromNode(*root_node, object);
}

}


Object::StaticType Object::StaticType::instance;

Object::StaticType* Object::StaticType::Instance() {
    return &instance;
}

Object::StaticType::StaticType() {
    zaf::internal::ReflectionManager::Instance().RegisterType(this);
}

ObjectType* Object::StaticType::GetBase() const {
    return nullptr;
}

const std::wstring& Object::StaticType::GetName() const {
    static std::wstring name{ L"Object" };
    return name;
}

std::shared_ptr<Object> Object::StaticType::CreateInstance() const {
    return Create<Object>();
}

ObjectParser* Object::StaticType::GetParser() const {
    static ObjectParser default_parser;
    return &default_parser;
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
    ParseObject(*GetType(), *this);
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

    return FromUTF8String(stream.str());
}


ObjectType* const Object::Type = StaticType::Instance();

}