#pragma once

#include <ObjIdl.h>
#include <optional>
#include <zaf/base/com_object.h>
#include <zaf/clipboard/data_descriptor.h>

namespace zaf::clipboard {

class FormatEnumerator : public COMObject<IEnumFORMATETC> {
public:
    explicit FormatEnumerator(COMPtr<IEnumFORMATETC> inner);

    std::optional<DataDescriptor> Next();
    void Reset();
};

}