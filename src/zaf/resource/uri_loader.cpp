#include <zaf/resource/uri_loader.h>
#include <zaf/resource/internal/default_uri_loader.h>

namespace zaf {

std::shared_ptr<URILoader> URILoader::DefaultLoader() {

    static auto instance = std::make_shared<internal::DefaultURILoader>();
    return instance;
}

}