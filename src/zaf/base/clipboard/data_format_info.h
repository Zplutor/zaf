#pragma once

#include <objidl.h>
#include <zaf/base/clipboard/clipboard_format.h>
#include <zaf/base/clipboard/data_medium.h>

namespace zaf {

class DataFormatInfo {
public:
    ClipboardFormat Format() const {
        return static_cast<ClipboardFormat>(inner_.cfFormat);
    }

    void SetFormat(ClipboardFormat format) {
        inner_.cfFormat = static_cast<CLIPFORMAT>(format);
    }

    DataMedium Medium() const {
        return static_cast<DataMedium>(inner_.tymed);
    }

    void SetMedium(DataMedium medium) {
        inner_.tymed = static_cast<DWORD>(medium);
    }

    const FORMATETC& Inner() const {
        return inner_;
    }

private:
    FORMATETC inner_{};
};

}