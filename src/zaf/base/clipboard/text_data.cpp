#include <zaf/base/clipboard/text_data.h>
#include <zaf/base/global_mem.h>

namespace zaf {

TextData::TextData(std::wstring text) : text_(std::move(text)) {

}


DataMedium TextData::SaveToMedium(const DataFormat& format) {

    std::size_t memory_size = text_.size() * sizeof(wchar_t);
    auto memory = GlobalMem::Alloc(memory_size, GlobalMemFlags::Movable);
    {
        auto lock = memory.Lock();
        std::memcpy(lock.Pointer(), text_.data(), memory_size);
    }
    return DataMedium::FromGlobalMem(std::move(memory));
}


void TextData::LoadFromMedium(const DataFormat& format, DataMedium medium) {

    medium.VisitGlobalMem([this](const GlobalMem& global_mem) {
        auto lock = global_mem.Lock();
        text_ = reinterpret_cast<const wchar_t*>(lock.Pointer());
    });
}

}