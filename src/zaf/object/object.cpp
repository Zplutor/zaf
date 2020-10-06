#include <zaf/object/object.h>
#include <sstream>
#include <zaf/base/string/encoding_conversion.h>

namespace zaf {

const std::shared_ptr<Object>& Object::Empty() {

    static auto empty_object = std::make_shared<Object>();
    return empty_object;
}


std::wstring Object::ToString() const {

    std::ostringstream stream;
    stream << this << ' ' << typeid(*this).name();

    return FromUtf8String(stream.str());
}

}