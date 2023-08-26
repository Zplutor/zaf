#include <gtest/gtest.h>
#include <zaf/crypto/sha.h>

using namespace zaf::crypto;

TEST(SHATest, SHA1) {

    SHA1 sha1;
    std::string input("zaf");
    sha1.Update(input.data(), input.size());
    ASSERT_EQ(sha1.Finish().ToHexString(), L"EAEE311EB87BD13BD59F3041486A678CF556905C");
}


TEST(SHATest, SHA256) {

    SHA256 sha256;
    std::string input("zaf");
    sha256.Update(input.data(), input.size());
    ASSERT_EQ(
        sha256.Finish().ToHexString(),
        L"AD38816045829DFD1C07DE7D18954A0D63FF3F7BD7602ADBA33A0E95E7F5D4F6");
}


TEST(SHATest, SHA384) {

    SHA384 sha384;
    std::string input("zaf");
    sha384.Update(input.data(), input.size());
    ASSERT_EQ(
        sha384.Finish().ToHexString(),
        L"B26397FABBB5BD1B2EE81248C03EA6EC07DB6FFA9C570EC5"
        L"3F9CE8E9F46F57625793CBFCA5C6C72CF2FE0CBBA188D5CF");
}


TEST(SHATest, SHA512) {

    SHA512 sha512;
    std::string input("zaf");
    sha512.Update(input.data(), input.size());
    ASSERT_EQ(
        sha512.Finish().ToHexString(),
        L"029546FA5D6DC7FA04E24C5D4073DCD6670F83FB2F26D545716F68E928C88588"
        L"989CF466D1BF2A5FBDE1DF9A5FE6D2E1715CEEBBD4A62C45ACCB2403599450D5");
}