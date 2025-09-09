#include <zaf/dynamic/boxing/boolean.h>
#include <zaf/dynamic/boxing/internal/boxed_represent_equal.h>

namespace zaf {

ZAF_OBJECT_IMPL(Boolean);


bool Boolean::IsEqual(const dynamic::Object& other) const {
    return internal::BoxedRepresentEqual(*this, other);
}    


std::size_t Boolean::Hash() const {
    return std::hash<typename Boolean::ValueType>()(Value());                              
}


std::wstring Boolean::ToString() const {
    if (Value()) {
        return L"true";
    }
    else {
        return L"false";
    }
}

}