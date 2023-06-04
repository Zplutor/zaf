#pragma once

#include <ObjIdl.h>
#include <optional>
#include <zaf/base/com_object.h>
#include <zaf/clipboard/format.h>

namespace zaf::clipboard {

class FormatEnumerator {
public:
    explicit FormatEnumerator(COMObject<IEnumFORMATETC> inner);

    std::optional<Format> Next();
    void Reset();

private:
    COMObject<IEnumFORMATETC> inner_;
};

}