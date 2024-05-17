#include <zaf/clipboard/text_data.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/base/global_mem.h>
#include <zaf/object/type_definition.h>

namespace zaf::clipboard {

ZAF_DEFINE_TYPE(TextData)
ZAF_DEFINE_TYPE_END;

TextData::TextData(std::wstring text) : text_(std::move(text)) {

}


Medium TextData::SaveToMedium(const DataDescriptor& data_descriptor) {

    if (data_descriptor.FormatType() != FormatType::Text) {
        throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
    }

    if (data_descriptor.Aspect() != DataAspect::Content) {
        throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
    }

    if (!HasFlag(data_descriptor.MediumTypes(), MediumType::GlobalMem)) {
        throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
    }

    return Medium::FromGlobalMem(GlobalMem::FromString(text_, GlobalMemFlags::Movable));
}


void TextData::LoadFromMedium(const Medium& medium, const DataDescriptor& data_descriptor) {

    if (data_descriptor.FormatType() != FormatType::Text) {
        throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
    }

    if (data_descriptor.Aspect() != DataAspect::Content) {
        throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
    }

    if (medium.Type() != MediumType::GlobalMem) {
        throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
    }

    medium.VisitGlobalMem([this](const GlobalMem& global_mem) {
        auto lock = global_mem.Lock();
        text_ = reinterpret_cast<const wchar_t*>(lock.Pointer());
    });
}

}