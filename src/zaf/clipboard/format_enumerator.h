#pragma once

#include <ObjIdl.h>
#include <optional>
#include <zaf/base/com_ptr.h>
#include <zaf/clipboard/format.h>

namespace zaf::clipboard {

class FormatEnumerator {
public:
    explicit FormatEnumerator(COMPtr<IEnumFORMATETC> inner);

    std::optional<Format> Next();
    void Reset();

private:
    COMPtr<IEnumFORMATETC> inner_;
};

}