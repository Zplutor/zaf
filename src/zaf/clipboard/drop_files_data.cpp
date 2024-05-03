#include <zaf/clipboard/drop_files_data.h>
#include <ShlObj.h>
#include <zaf/base/error/invalid_data_error.h>
#include <zaf/base/error/not_supported_error.h>
#include <zaf/object/type_definition.h>

namespace zaf::clipboard {

ZAF_DEFINE_TYPE(DropFilesData)
ZAF_DEFINE_TYPE_END;

DropFilesData::DropFilesData(std::vector<std::filesystem::path> file_paths) : 
    paths_(std::move(file_paths)) {

}


Medium DropFilesData::SaveToMedium(const Format& format) {

    if (format.Type() != FormatType::DropFiles || format.MediumType() != MediumType::GlobalMem) {
        throw NotSupportedError{ ZAF_SOURCE_SITE() };
    }

    if (paths_.empty()) {
        throw InvalidDataError{ ZAF_SOURCE_SITE() };
    }

    std::wstring paths_buffer;
    for (const auto& each_path : paths_) {

        if (!each_path.is_absolute()) {
            throw InvalidDataError{ ZAF_SOURCE_SITE() };
        }

        paths_buffer += each_path;
        paths_buffer += L'\0';
    }
    paths_buffer += L'\0';

    auto total_buffer_size = sizeof(DROPFILES) + paths_buffer.size() * sizeof(wchar_t);
    auto global_mem = GlobalMem::Alloc(total_buffer_size, GlobalMemFlags::Movable);
    {
        auto lock = global_mem.Lock();

        auto drop_files_info = reinterpret_cast<DROPFILES*>(lock.Pointer());
        drop_files_info->pFiles = sizeof(DROPFILES);
        drop_files_info->pt = {};
        drop_files_info->fNC = FALSE;
        drop_files_info->fWide = TRUE;

        std::memcpy(drop_files_info + 1, paths_buffer.data(), total_buffer_size);
    }

    return Medium::FromGlobalMem(std::move(global_mem));
}


void DropFilesData::LoadFromMedium(const Format& format, const Medium& medium) {

    if (format.Type() != FormatType::DropFiles || medium.Type() != MediumType::GlobalMem) {
        throw NotSupportedError{ ZAF_SOURCE_SITE() };
    }

    paths_.clear();

    medium.VisitGlobalMem([this](const GlobalMem& global_mem) {
        
        auto lock = global_mem.Lock();
        auto d = (DROPFILES*)(lock.Pointer());
        auto drop_files_info = reinterpret_cast<HDROP>(lock.Pointer());

        UINT file_count = DragQueryFile(drop_files_info, -1, nullptr, 0);
        for (UINT index = 0; index < file_count; ++index) {

            auto path_length = DragQueryFile(drop_files_info, index, nullptr, 0);
            //The returned value doesn't include the null terminator.
            auto buffer_length = path_length + 1;

            std::wstring path(buffer_length, '\0');
            path_length = DragQueryFile(drop_files_info, index, &path[0], buffer_length);
            path.resize(path_length);

            paths_.push_back(std::move(path));
        }
    });
}

}