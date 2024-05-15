#include <zaf/clipboard/text_data.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/base/global_mem.h>
#include <zaf/object/type_definition.h>

namespace zaf::clipboard {

ZAF_DEFINE_TYPE(TextData)
ZAF_DEFINE_TYPE_END;

TextData::TextData(std::wstring text) : text_(std::move(text)) {

}


Medium TextData::SaveToMedium(const Format& format) {

    if (format.Type() == FormatType::Text && format.MediumType() == MediumType::GlobalMem) {
        return Medium::FromGlobalMem(GlobalMem::FromString(text_, GlobalMemFlags::Movable));
    }

    throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
}


void TextData::LoadFromMedium(FormatType format_type, const Medium& medium) {

    if (format_type == FormatType::Text && medium.Type() == MediumType::GlobalMem) {

        medium.VisitGlobalMem([this](const GlobalMem& global_mem) {
            auto lock = global_mem.Lock();
            text_ = reinterpret_cast<const wchar_t*>(lock.Pointer());
        });
        return;
    }

    throw InvalidOperationError{ ZAF_SOURCE_LOCATION() };
}

}