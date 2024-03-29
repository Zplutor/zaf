#pragma once

#include <filesystem>
#include <vector>
#include <zaf/clipboard/clipboard_data.h>

namespace zaf::clipboard {

class DropFilesData : public ClipboardData {
public:
    ZAF_DECLARE_TYPE;

public:
    DropFilesData() = default;
    explicit DropFilesData(std::vector<std::filesystem::path> file_paths);

    const std::vector<std::filesystem::path>& GetFilePaths() const {
        return paths_;
    }

    Medium SaveToMedium(const Format& format) override;
    void LoadFromMedium(const Format& format, const Medium& medium) override;

private:
    std::vector<std::filesystem::path> paths_;
};

}