#include <zaf/control/textual/styled_text_data.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/xml/xml_serialization.h>

using namespace zaf::clipboard;

namespace zaf::textual {

clipboard::FormatType StyledTextData::FormatType() {
    static clipboard::FormatType format_type = MakeSharedFormatType(L"zaf.StyledText");
    return format_type;
}


StyledTextData::StyledTextData(StyledText styled_text) : 
    styled_text_(std::move(styled_text)) {

}


Medium StyledTextData::SaveToMedium(const DataDescriptor& data_descriptor) const {

    if (!HasFlag(data_descriptor.MediumTypes(), MediumType::GlobalMem)) {
        throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
    }

    if (data_descriptor.Aspect() != DataAspect::Content) {
        throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
    }

    if (data_descriptor.FormatType() == FormatType::Text) {
        return Medium::FromString(styled_text_.Text());
    }
    else if (data_descriptor.FormatType() == FormatType()) {
        return SaveToStyledTextFormat();
    }

    throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
}


Medium StyledTextData::SaveToStyledTextFormat() const {

    auto stream = XMLSerializeToMemoryStream(styled_text_, { .code_page = CodePage::UTF8 });

    auto global_mem = GlobalMem::Alloc(stream.GetSize(), GlobalMemFlags::Movable);
    {
        auto lock = global_mem.Lock();
        std::memcpy(lock.Pointer(), stream.GetUnderlyingBuffer(), stream.GetSize());
    }

    return Medium::FromGlobalMem(std::move(global_mem));
}


void StyledTextData::LoadFromMedium(
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


StyledText StyledTextData::LoadStyledText(
    const Medium& medium,
    const DataDescriptor& data_descriptor) {

    if (data_descriptor.FormatType() == clipboard::FormatType::Text) {
        return StyledText{ medium.ToString() };
    }
    else if (data_descriptor.FormatType() == FormatType()) {
        return LoadWithStyledTextFormat(medium);
    }

    throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
}


StyledText StyledTextData::LoadWithStyledTextFormat(const Medium& medium) {

    StyledText result;
    medium.VisitGlobalMem([&result](const GlobalMem& global_mem) {

        auto lock = global_mem.Lock();
        auto stream = Stream::CreateOnMemory(lock.Pointer(), global_mem.Size());
        XMLDeserialize(result, stream, { .code_page = CodePage::UTF8 });
    });

    return result;
}

}