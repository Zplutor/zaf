#pragma once

#include <zaf/object/boxing/internal/boxed_represent.h>
#include <zaf/object/boxing/internal/built_in_box_types.h>
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
ZAF_OBJECT_END;

__ZAF_INTERNAL_DEFINE_BUILT_IN_BOXED_TYPE(bool, Boolean)

}