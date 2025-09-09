#pragma once

#include <zaf/dynamic/boxing/boolean_parser.h>
#include <zaf/dynamic/boxing/custom_boxing_traits.h>
#include <zaf/dynamic/boxing/internal/boxed_represent.h>
#include <zaf/dynamic/object.h>

namespace zaf {

class Boolean : public dynamic::Object, public internal::BoxedRepresent<bool> {
public:
    ZAF_OBJECT;

    using BoxedRepresent<bool>::BoxedRepresent;

    bool IsEqual(const dynamic::Object& other) const override;
    std::size_t Hash() const override;
    std::wstring ToString() const override;
};

ZAF_OBJECT_BEGIN(Boolean);
ZAF_OBJECT_PARSER(BooleanParser);
ZAF_OBJECT_END;

template<>
struct zaf__CustomBoxingTraits<bool> {

    using BoxedType = Boolean;

    static std::shared_ptr<Boolean> Box(bool value) {
        return std::make_shared<Boolean>(value);
    }

    static const bool* Unbox(const Boolean& object) {
        return &object.Value();
    }
};

}