#include <zaf/base/clipboard/text_format.h>
#include <zaf/base/global_mem.h>

namespace zaf {

TextDataFormat::TextDataFormat(std::wstring text) : text_(std::move(text)) {

}


FORMATETC TextDataFormat::GetFORMATECT() {

    FORMATETC result{};
    result.cfFormat = CF_UNICODETEXT;
    result.tymed = TYMED_HGLOBAL;
    result.dwAspect = DVASPECT_CONTENT;
    result.lindex = -1;
    return result;
}


STGMEDIUM TextDataFormat::GetSTGMEDIUM() {

    std::size_t memory_size = text_.size() * sizeof(wchar_t);
    auto memory = GlobalMem::Alloc(memory_size, GlobalMemFlags::Movable);
    {
        auto lock = memory.Lock();
        std::memcpy(lock.Pointer(), text_.data(), memory_size);
    }

    STGMEDIUM result{};
    result.tymed = TYMED_HGLOBAL;
    result.hGlobal = memory.Detach();
    return result;
}

}