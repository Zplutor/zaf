#pragma once

#include <filesystem>
#include <vector>
#include <zaf/clipboard/clipboard_data.h>

namespace zaf::clipboard {

class DropFilesData : public ClipboardData {
public:
    DropFilesData() = default;
    explicit DropFilesData(std::vector<std::filesystem::path> file_paths);

    const std::vector<std::filesystem::path>& GetFilePaths() const {
        return paths_;
    }

    Medium SaveToMedium(const DataDescriptor& data_descriptor) const override;
    void LoadFromMedium(const Medium& medium, const DataDescriptor& data_descriptor) override;

private:
    std::vector<std::filesystem::path> paths_;
};

}