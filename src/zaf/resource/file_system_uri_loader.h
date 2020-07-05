#pragma once

#include <zaf/resource/uri_loader.h>

namespace zaf {

class FileSystemUriLoader : public UriLoader {
public:
    FileSystemUriLoader(const std::filesystem::path& base_path);

    Stream Load(const std::wstring& uri) override;

private:
    std::filesystem::path base_path_;
};

}