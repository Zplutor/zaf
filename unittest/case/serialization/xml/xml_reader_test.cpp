#include <gtest/gtest.h>
#include <zaf/base/stream.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/xml/xml_reader.h>

namespace {

std::shared_ptr<zaf::DataNode> ConvertDataNodeFromXmlString(const std::string& string) {

    zaf::XmlReader xml_reader;
    std::error_code error_code;
    return xml_reader.Read(string, error_code);
}

}

TEST(XmlReader, ReadStringNode) {

    auto test = [](const std::string& xml_string, const std::wstring& expected_string) {
    
        auto data_node = ConvertDataNodeFromXmlString(xml_string);
        if (data_node == nullptr) {
            return false;
        }

        if (! data_node->IsString()) {
            return false;
        }

        return data_node->GetString() == expected_string;
    };

    ASSERT_TRUE(test("<string />", L""));
    ASSERT_TRUE(test("<string></string>", L""));
    ASSERT_TRUE(test("<string>zaf</string>", L"zaf"));
}


TEST(XmlReader, ReadBooleanNode) {

    auto test = [](const char* xml_string, bool expected_value) {

        auto data_node = ConvertDataNodeFromXmlString(xml_string);
        if (data_node == nullptr) {
            return false;
        }

        if (! data_node->IsBoolean()) {
            return false;
        }
        
        return data_node->GetBoolean() == expected_value;
    };

    ASSERT_TRUE(test("<boolean />", false));
    ASSERT_TRUE(test("<boolean></boolean>", false));
    ASSERT_TRUE(test("<boolean>true</boolean>", true)); 
    ASSERT_TRUE(test("<boolean>false</boolean>", false));
}


TEST(XmlReader, ReadNumberNode) {

    auto test = [](const char* xml_string, zaf::Number::Tag tag, const std::function<bool(zaf::Number)>& check_value) {
    
        auto data_node = ConvertDataNodeFromXmlString(xml_string);
        if (data_node == nullptr) {
            return false;
        }

        if (! data_node->IsNumber()) {
            return false;
        }

        if (data_node->GetNumber().GetTag() != tag) {
            return false;
        }

        return check_value(data_node->GetNumber());
    };

    ASSERT_TRUE(test("<number />", zaf::Number::Tag::SignedInteger, [](zaf::Number number) {
        return number.GetValue<std::int64_t>() == 0;
    }));

    ASSERT_TRUE(test("<number></number>", zaf::Number::Tag::SignedInteger, [](zaf::Number number) {
        return number.GetValue<std::int64_t>() == 0;
    }));

    ASSERT_TRUE(test("<number>-19</number>", zaf::Number::Tag::SignedInteger, [](zaf::Number number) {
        return number.GetValue<std::int64_t>() == -19;
    }));

    ASSERT_TRUE(test("<number>18446744073709551615</number>", zaf::Number::Tag::UnsignedInteger, [](zaf::Number number) {
        return number.GetValue<std::uint64_t>() == 18446744073709551615;
    }));
    
    ASSERT_TRUE(test("<number>19.19</number>", zaf::Number::Tag::FloatingPoint, [](zaf::Number number) {
        return number.GetValue<double>() == 19.19;
    }));
}


TEST(XmlReader, ReadObject) {

    auto data_node = ConvertDataNodeFromXmlString("<object />");
    ASSERT_NE(data_node, nullptr);
    ASSERT_TRUE(data_node->IsObject());
    ASSERT_EQ(data_node->GetChildCount(), 0);

    data_node = ConvertDataNodeFromXmlString("<object></object>");
    ASSERT_NE(data_node, nullptr);
    ASSERT_TRUE(data_node->IsObject());
    ASSERT_EQ(data_node->GetChildCount(), 0);

    const char* xml_string = 
        "<object>"
        "<string key=\"S\">zaf</string>"
        "<boolean key=\"B\">true</boolean>"
        "<number key=\"N\">2</number>"
        "<object key=\"O\">"
        "<string key=\"SS\">ffff</string>"
        "</object>"
        "<array key=\"A\">"
        "<string>ara</string>"
        "</array>"
        "</object>";
    data_node = ConvertDataNodeFromXmlString(xml_string);
    ASSERT_NE(data_node, nullptr);
    ASSERT_TRUE(data_node->IsObject());

    auto field_node = data_node->GetChild(L"S");
    ASSERT_NE(field_node, nullptr);
    ASSERT_TRUE(field_node->IsString());
    ASSERT_EQ(field_node->GetString(), L"zaf");

    field_node = data_node->GetChild(L"B");
    ASSERT_NE(field_node, nullptr);
    ASSERT_TRUE(field_node->IsBoolean());
    ASSERT_EQ(field_node->GetBoolean(), true);

    field_node = data_node->GetChild(L"N");
    ASSERT_NE(field_node, nullptr);
    ASSERT_TRUE(field_node->IsNumber());
    ASSERT_EQ(field_node->GetInt32(), 2);

    field_node = data_node->GetChild(L"O");
    ASSERT_NE(field_node, nullptr);
    ASSERT_TRUE(field_node->IsObject());

    auto field_field_node = field_node->GetChild(L"SS");
    ASSERT_NE(field_field_node, nullptr);
    ASSERT_TRUE(field_field_node->IsString());
    ASSERT_EQ(field_field_node->GetString(), L"ffff");

    field_node = data_node->GetChild(L"A");
    ASSERT_NE(field_node, nullptr);
    ASSERT_TRUE(field_node->IsArray());

    auto field_element_node = field_node->GetChild(0);
    ASSERT_NE(field_element_node, nullptr);
    ASSERT_EQ(field_element_node->GetString(), L"ara");
}


TEST(XmlReader, ReadArray) {

    auto data_node = ConvertDataNodeFromXmlString("<array />");
    ASSERT_NE(data_node, nullptr);
    ASSERT_TRUE(data_node->IsArray());
    ASSERT_EQ(data_node->GetChildCount(), 0);

    const char* xml = 
        "<array>"
        "<string>zaf</string>"
        "<boolean>false</boolean>"
        "<number>343.3</number>"
        "<object />"
        "<array />"
        "</array>";
    data_node = ConvertDataNodeFromXmlString(xml);
    ASSERT_NE(data_node, nullptr);
    ASSERT_TRUE(data_node->IsArray());
    ASSERT_EQ(data_node->GetChildCount(), 5);

    auto child_node = data_node->GetChild(0);
    ASSERT_NE(child_node, nullptr);
    ASSERT_TRUE(child_node->IsString());
    ASSERT_EQ(child_node->GetString(), L"zaf");

    child_node = data_node->GetChild(1);
    ASSERT_NE(child_node, nullptr);
    ASSERT_TRUE(child_node->IsBoolean());
    ASSERT_EQ(child_node->GetBoolean(), false);

    child_node = data_node->GetChild(2);
    ASSERT_NE(child_node, nullptr);
    ASSERT_TRUE(child_node->IsNumber());
    ASSERT_EQ(child_node->GetNumber().GetValue<double>(), 343.3);

    child_node = data_node->GetChild(3);
    ASSERT_NE(child_node, nullptr);
    ASSERT_TRUE(child_node->IsObject());
    
    child_node = data_node->GetChild(4);
    ASSERT_NE(child_node, nullptr);
    ASSERT_TRUE(child_node->IsArray());
}