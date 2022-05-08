#include <gtest/gtest.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/resource/uri_loader.h>
#include "utility/assert.h"

using namespace zaf;

TEST(DefaultURILoaderTest, InvalidURI) {

    ASSERT_THROW_ERRC(
        URILoader::DefaultLoader()->Load(L"", 96.f),
        BasicErrc::InvalidValue);
}