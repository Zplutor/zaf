#include <gtest/gtest.h>
#include <zaf/base/error/com_error.h>
#include <zaf/xml/xml_error.h>
#include <zaf/xml/xml_reader.h>

using namespace zaf;

namespace {

XMLReader CreateReader(std::string_view xml) {
    auto stream = Stream::FromMemory(xml.data(), xml.size());
    return XMLReader{ stream };
}

}

TEST(XMLReaderTest, Read) {

    auto reader = CreateReader("<E></E>");

    ASSERT_TRUE(reader.Read());
    ASSERT_EQ(reader.GetNodeType(), XMLNodeType::ElementStart);
    ASSERT_EQ(reader.GetName(), L"E");

    ASSERT_TRUE(reader.Read());
    ASSERT_EQ(reader.GetNodeType(), XMLNodeType::ElementEnd);
    ASSERT_EQ(reader.GetName(), L"E");

    ASSERT_FALSE(reader.Read());
}


TEST(XMLReaderTest, ReadXMLDeclaration) {

    auto reader = CreateReader(R"(<?xml version="1.0" ?><E></E>)");
    ASSERT_NO_THROW(reader.ReadXMLDeclaration());
    ASSERT_EQ(reader.GetNodeType(), XMLNodeType::ElementStart);
    ASSERT_EQ(reader.GetName(), L"E");
}


TEST(XMLReaderTest, ReadUntilElement) {

    {
        auto reader = CreateReader("<E />");
        ASSERT_NO_THROW(reader.ReadUntilElement(L"E"));
        ASSERT_EQ(reader.GetNodeType(), XMLNodeType::ElementStart);
        ASSERT_EQ(reader.GetName(), L"E");
    }

    {
        auto reader = CreateReader("<E></E>");
        ASSERT_NO_THROW(reader.ReadUntilElement(L"E"));
        ASSERT_EQ(reader.GetNodeType(), XMLNodeType::ElementStart);
        ASSERT_EQ(reader.GetName(), L"E");
    }

    {
        auto reader = CreateReader("    <E />");
        ASSERT_NO_THROW(reader.ReadUntilElement(L"E"));
        ASSERT_EQ(reader.GetNodeType(), XMLNodeType::ElementStart);
        ASSERT_EQ(reader.GetName(), L"E");
    }
    
    {
        auto reader = CreateReader(R"(<?xml version="1.0" ?><E></E>)");
        ASSERT_THROW(reader.ReadUntilElement(L"E"), XMLError);
    }

    {
        auto reader = CreateReader(R"(<E></E>)");
        ASSERT_THROW(reader.ReadUntilElement(L"B"), XMLError);
    }

    //Call ReadUntilElement() at the end of reader.
    {
        auto reader = CreateReader("<E/>");
        ASSERT_TRUE(reader.Read());
        ASSERT_FALSE(reader.Read());
        ASSERT_THROW(reader.ReadUntilElement(L"E"), XMLError);
    }
}