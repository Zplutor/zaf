#pragma once

#include <memory>
#include <vector>
#include <ObjIdl.h>
#include <zaf/base/non_copyable.h>

namespace zaf {

class DataFormat : NonCopyableNonMovable {
public:
    DataFormat() = default;
    virtual ~DataFormat() = default;

    virtual FORMATETC GetFORMATECT() = 0;
    virtual STGMEDIUM GetSTGMEDIUM() = 0;
};

using DataFormatList = std::vector<std::shared_ptr<DataFormat>>;

}