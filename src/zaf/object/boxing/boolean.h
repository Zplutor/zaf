#pragma once

#include <zaf/object/boxing/boolean_parser.h>
#include <zaf/object/boxing/custom_boxed_type.h>
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
struct GetCustomBoxedType<bool> {
    using type = Boolean;
};

}