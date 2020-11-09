#include <zaf/resource/default_relative_uri_loader.h>
#include <zaf/application.h>
#include <zaf/resource/file_system_uri_loader.h>

namespace zaf {

std::shared_ptr<DefaultRelativeUriLoader> DefaultRelativeUriLoader::GetInstance() {

    static std::shared_ptr<DefaultRelativeUriLoader> instance{ new DefaultRelativeUriLoader{} };
    return instance;
}


Stream DefaultRelativeUriLoader::Load(const std::wstring& uri) {

    auto directory_path = Application::Instance().GetExeDirectoryPath();

    FileSystemUriLoader file_system_uri_loader{ directory_path };
    return file_system_uri_loader.Load(uri);
}

}