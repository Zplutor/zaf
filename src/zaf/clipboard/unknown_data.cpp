#include <zaf/clipboard/unknown_data.h>
#include <zaf/base/error/invalid_operation_error.h>

namespace zaf::clipboard {

clipboard::Medium UnknownData::SaveToMedium(const clipboard::DataDescriptor& data_descriptor) {

    if (data_descriptor_ != data_descriptor) {
        throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
    }

    return medium_;
}


void UnknownData::LoadFromMedium(
    const clipboard::Medium& medium, 
    const clipboard::DataDescriptor& data_descriptor) {

    data_descriptor_ = data_descriptor;
    medium_ = medium;
}

}