#include <zaf/resource/default_relative_uri_loader.h>

namespace zaf {

std::shared_ptr<DefaultRelativeUriLoader> DefaultRelativeUriLoader::GetInstance() {

    static std::shared_ptr<DefaultRelativeUriLoader> instance{ new DefaultRelativeUriLoader{} };
    return instance;
}


Stream DefaultRelativeUriLoader::Load(const std::wstring& uri) {

    wchar_t buffer[MAX_PATH]{};
    GetModuleFileName(nullptr, buffer, MAX_PATH);

    std::filesystem::path path{ buffer };
    return Stream::FromFile(path.parent_path() / uri);
}

}