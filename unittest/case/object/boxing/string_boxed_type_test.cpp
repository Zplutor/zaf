#include <gtest/gtest.h>
#include <zaf/object/boxing/boxing.h>
#include <zaf/object/boxing/string.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_reader.h>

TEST(StringBoxedTypeTest, StringIsEqual) {

    std::string s1 = "abcde";
    std::string s2 = "abcde";
    std::string s3 = "abcdef";

    auto boxed1 = zaf::Box(s1);
    auto boxed2 = zaf::Box(s2);
    auto boxed3 = zaf::Box(s3);

    ASSERT_TRUE(boxed1->IsEqual(*boxed2));
    ASSERT_FALSE(boxed1->IsEqual(*boxed3));
}


TEST(StringBoxedTypeTest, WideStringIsEqual) {

    std::wstring s1 = L"abcde";
    std::wstring s2 = L"abcde";
    std::wstring s3 = L"abcdef";

    auto boxed1 = zaf::Box(s1);
    auto boxed2 = zaf::Box(s2);
    auto boxed3 = zaf::Box(s3);

    ASSERT_TRUE(boxed1->IsEqual(*boxed2));
    ASSERT_FALSE(boxed1->IsEqual(*boxed3));
}


TEST(StringBoxedTypeTest, StringHash) {

    auto boxed1 = zaf::Box(std::string{ "hash"});
    auto boxed2 = zaf::Box(std::string{ "hash" });
    auto boxed3 = zaf::Box(std::string{ "HASH" });

    ASSERT_EQ(boxed1->Hash(), boxed2->Hash());
    ASSERT_NE(boxed1->Hash(), boxed3->Hash());
    ASSERT_NE(boxed2->Hash(), boxed3->Hash());
}


TEST(StringBoxedTypeTest, WideStringHash) {

    auto boxed1 = zaf::Box(std::wstring{ L"hash" });
    auto boxed2 = zaf::Box(std::wstring{ L"hash" });
    auto boxed3 = zaf::Box(std::wstring{ L"HASH" });

    ASSERT_EQ(boxed1->Hash(), boxed2->Hash());
    ASSERT_NE(boxed1->Hash(), boxed3->Hash());
    ASSERT_NE(boxed2->Hash(), boxed3->Hash());
}


TEST(StringBoxedTypeTest, StringParse) {

    zaf::String string;

    auto parser = zaf::String::Type->GetParser();
    parser->ParseFromAttribute(L"an attribute", string);
    ASSERT_EQ(string.Value(), "an attribute");

    auto node = zaf::XamlReader::FromString("<String>a node</String>")->Read();
    parser->ParseFromNode(*node, string);
    ASSERT_EQ(string.Value(), "a node");
}


TEST(StringBoxedTypeTest, WideStringParse) {

    zaf::WideString string;

    auto parser = zaf::WideString::Type->GetParser();
    parser->ParseFromAttribute(L"an attribute", string);
    ASSERT_EQ(string.Value(), L"an attribute");

    auto node = zaf::XamlReader::FromString("<String>a node</String>")->Read();
    parser->ParseFromNode(*node, string);
    ASSERT_EQ(string.Value(), L"a node");
}