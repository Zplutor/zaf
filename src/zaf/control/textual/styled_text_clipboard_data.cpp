#include <zaf/control/textual/styled_text_clipboard_data.h>
#include <zaf/object/type_definition.h>

using namespace zaf::clipboard;

namespace zaf::textual {

ZAF_DEFINE_TYPE(StyledTextClipboardData)
ZAF_DEFINE_TYPE_END;


clipboard::FormatType StyledTextClipboardData::StyledTextFormatType() {
    static FormatType format_type = MakeSharedFormatType(L"zaf.StyledText");
    return format_type;
}


StyledTextClipboardData::StyledTextClipboardData(StyledText styled_text) : 
    styled_text_(std::move(styled_text)) {

}


Medium StyledTextClipboardData::SaveToMedium(const DataDescriptor& data_descriptor) {

    if (!HasFlag(data_descriptor.MediumTypes(), MediumType::GlobalMem)) {
        throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
    }

    if (data_descriptor.Aspect() != DataAspect::Content) {
        throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
    }

    if (data_descriptor.FormatType() == FormatType::Text) {
        return Medium::FromString(styled_text_.Text());
    }
    else if (data_descriptor.FormatType() == StyledTextFormatType()) {
        return SaveToStyledTextFormat();
    }

    throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
}


Medium StyledTextClipboardData::SaveToStyledTextFormat() const {

    //TODO
    return Medium::FromString(styled_text_.Text());
}


void StyledTextClipboardData::LoadFromMedium(
    const Medium& medium,
    const DataDescriptor& data_descriptor) {

    if (medium.Type() != MediumType::GlobalMem) {
        throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
    }

    if (data_descriptor.Aspect() != DataAspect::Content) {
        throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
    }

    styled_text_ = LoadStyledText(medium, data_descriptor);
}


StyledText StyledTextClipboardData::LoadStyledText(
    const Medium& medium,
    const DataDescriptor& data_descriptor) {

    if (data_descriptor.FormatType() == clipboard::FormatType::Text) {
        return StyledText{ medium.ToString() };
    }
    else if (data_descriptor.FormatType() == StyledTextFormatType()) {
        return LoadWithStyledTextFormat(medium);
    }

    throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
}


StyledText StyledTextClipboardData::LoadWithStyledTextFormat(const Medium& medium) {

    //TODO
    return StyledText{ medium.ToString() };
}

}