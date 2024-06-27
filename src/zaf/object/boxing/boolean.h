#pragma once

#include <zaf/object/boxing/boolean_parser.h>
#include <zaf/object/boxing/custom_boxing_traits.h>
#include <zaf/object/boxing/internal/boxed_represent.h>
#include <zaf/object/object.h>

namespace zaf {

class Boolean : public Object, public internal::BoxedRepresent<bool> {
public:
    ZAF_OBJECT;

    using BoxedRepresent<bool>::BoxedRepresent;

    bool IsEqual(const Object& other) const override;
    std::size_t Hash() const override;
    std::wstring ToString() const override;
};

ZAF_OBJECT_BEGIN(Boolean);
ZAF_OBJECT_PARSER(BooleanParser);
ZAF_OBJECT_END;

template<>
struct CustomBoxingTraits<bool> {

    using BoxedType = Boolean;

    static std::shared_ptr<Boolean> Box(bool value) {
        return std::make_shared<Boolean>(value);
    }

    static const bool* Unbox(const Boolean& object) {
        return &object.Value();
    }
};

}