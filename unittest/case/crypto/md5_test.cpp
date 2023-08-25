#include <gtest/gtest.h>
#include <zaf/crypto/md5.h>

using namespace zaf::crypto;

TEST(MD5Test, MD5) {

    MD5 md5;

    std::string string("abcdefg");
    md5.Update(string.data(), string.length());

    auto hash = md5.Finish();

}