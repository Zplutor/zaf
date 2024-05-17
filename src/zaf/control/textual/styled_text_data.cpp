#include <zaf/control/textual/styled_text_data.h>
#include <zaf/object/type_definition.h>

namespace zaf::textual {

ZAF_DEFINE_TYPE(StyledTextData)
ZAF_DEFINE_TYPE_END;

clipboard::Medium StyledTextData::SaveToMedium(const clipboard::DataDescriptor& format) {
    return clipboard::Medium();
}


void StyledTextData::LoadFromMedium(
    const clipboard::Medium& medium,
    const clipboard::DataDescriptor& data_descriptor) {

}

}