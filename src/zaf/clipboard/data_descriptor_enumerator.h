#pragma once

/**
@file
    Defines the class zaf::clipboard::DataDescriptorEnumerator.
*/

#include <ObjIdl.h>
#include <optional>
#include <zaf/base/com_object.h>
#include <zaf/clipboard/data_descriptor.h>

namespace zaf::clipboard {

class DataDescriptorEnumerator : public COMObject<IEnumFORMATETC> {
public:
    explicit DataDescriptorEnumerator(COMPtr<IEnumFORMATETC> inner);

    std::optional<DataDescriptor> Next();
    void Reset();
};

}