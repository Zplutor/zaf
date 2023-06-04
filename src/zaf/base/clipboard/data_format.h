#pragma once

#include <objidl.h>
#include <zaf/base/clipboard/data_format_id.h>
#include <zaf/base/clipboard/data_medium_type.h>

namespace zaf {

class DataFormat {
public:
    DataFormat() {
        inner_.dwAspect = DVASPECT_CONTENT;
        inner_.lindex = -1;
    }

    DataFormat(DataFormatID id, DataMediumType medium_type) : DataFormat() {
        inner_.cfFormat = static_cast<CLIPFORMAT>(id);
        inner_.tymed = static_cast<DWORD>(medium_type);
    }

    explicit DataFormat(DataFormatID id) : DataFormat(id, DataMediumType::GlobalMem) {

    }

    explicit DataFormat(const FORMATETC& inner) : inner_(inner) {

    }

    DataFormatID ID() const {
        return static_cast<DataFormatID>(inner_.cfFormat);
    }

    DataMediumType MediumType() const {
        return static_cast<DataMediumType>(inner_.tymed);
    }

    const FORMATETC& Inner() const {
        return inner_;
    }

private:
    FORMATETC inner_{};
};

}