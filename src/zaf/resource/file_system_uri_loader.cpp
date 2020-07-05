#include <zaf/resource/file_system_uri_loader.h>

namespace zaf {

FileSystemUriLoader::FileSystemUriLoader(const std::filesystem::path& base_path) :
    base_path_(base_path) {

}


Stream FileSystemUriLoader::Load(const std::wstring& uri) {
    return Stream::FromFile(base_path_ / uri);
}

}