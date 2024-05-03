#include <gtest/gtest.h>
#include <zaf/resource/invalid_uri_error.h>
#include <zaf/resource/uri_loader.h>

using namespace zaf;

TEST(DefaultURILoaderTest, InvalidURI) {

    ASSERT_THROW(URILoader::DefaultLoader()->Load(L"", 96.f), InvalidURIError);
}