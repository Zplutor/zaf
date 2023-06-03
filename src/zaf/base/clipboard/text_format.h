#pragma once

#include <zaf/base/clipboard/data_format.h>

namespace zaf {

class TextDataFormat : public DataFormat {
public:
    explicit TextDataFormat(std::wstring text);

    FORMATETC GetFORMATECT() override;
    STGMEDIUM GetSTGMEDIUM() override;

private:
    std::wstring text_;
};

}