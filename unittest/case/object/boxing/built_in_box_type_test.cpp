#include <gtest/gtest.h>
#include <zaf/object/boxing/boxing.h>
#include <zaf/object/boxing/numeric.h>

TEST(BuiltInBoxTypeTest, NumericIsEqual) {

    int i1 = 12;
    int i2 = 12;
    int i3 = 13;

    auto boxed1 = zaf::Box(i1);
    auto boxed2 = zaf::Box(i2);
    auto boxed3 = zaf::Box(i3);

    ASSERT_TRUE(boxed1->IsEqual(*boxed2));
    ASSERT_FALSE(boxed1->IsEqual(*boxed3));
}


TEST(BuiltInBoxTypeTest, StringIsEqual) {

    std::string s1 = "abcde";
    std::string s2 = "abcde";
    std::string s3 = "abcdef";

    auto boxed1 = zaf::Box(s1);
    auto boxed2 = zaf::Box(s2);
    auto boxed3 = zaf::Box(s3);

    ASSERT_TRUE(boxed1->IsEqual(*boxed2));
    ASSERT_FALSE(boxed1->IsEqual(*boxed3));
}


TEST(BuiltInBoxTypeTest, WideStringIsEqual) {

    std::wstring s1 = L"abcde";
    std::wstring s2 = L"abcde";
    std::wstring s3 = L"abcdef";

    auto boxed1 = zaf::Box(s1);
    auto boxed2 = zaf::Box(s2);
    auto boxed3 = zaf::Box(s3);

    ASSERT_TRUE(boxed1->IsEqual(*boxed2));
    ASSERT_FALSE(boxed1->IsEqual(*boxed3));
}