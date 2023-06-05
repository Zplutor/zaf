#include <zaf/clipboard/text_data.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/base/global_mem.h>

namespace zaf::clipboard {

TextData::TextData(std::wstring text) : text_(std::move(text)) {

}


Medium TextData::SaveToMedium(const Format& format) {

    if (format.Type() == FormatType::Text && format.MediumType() == MediumType::GlobalMem) {
        return Medium::FromGlobalMem(GlobalMem::FromString(text_, GlobalMemFlags::Movable));
    }

    ZAF_THROW_ERRC(BasicErrc::Unsupported);
}


void TextData::LoadFromMedium(const Format& format, const Medium& medium) {

    if (format.Type() == FormatType::Text && format.MediumType() == MediumType::GlobalMem) {

        medium.VisitGlobalMem([this](const GlobalMem& global_mem) {
            auto lock = global_mem.Lock();
            text_ = reinterpret_cast<const wchar_t*>(lock.Pointer());
        });
        return;
    }

    ZAF_THROW_ERRC(BasicErrc::Unsupported);
}

}