#include <zaf/object/object.h>
#include <sstream>
#include <zaf/base/string/encoding_conversion.h>

namespace zaf {

const std::shared_ptr<Object>& Object::Empty() {

    static auto empty_object = std::make_shared<Object>();
    return empty_object;
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

}