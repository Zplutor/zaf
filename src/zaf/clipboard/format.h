#pragma once

#include <objidl.h>
#include <zaf/clipboard/format_type.h>
#include <zaf/clipboard/medium_type.h>

namespace zaf::clipboard {

class Format {
public:
    Format();
    Format(FormatType format_type, MediumType medium_type);
    explicit Format(FormatType format_type);
    explicit Format(const FORMATETC& inner);

    FormatType Type() const {
        return static_cast<FormatType>(inner_.cfFormat);
    }

    MediumType MediumType() const {
        return static_cast<clipboard::MediumType>(inner_.tymed);
    }

    const FORMATETC& Inner() const {
        return inner_;
    }

    bool operator==(const Format& other) const;
    bool operator==(const FORMATETC& other) const;

private:
    FORMATETC inner_{};
};

}