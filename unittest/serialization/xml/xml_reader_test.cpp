#include <gtest/gtest.h>
#include <zaf/base/stream.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/xml/xml_reader.h>

namespace {

std::shared_ptr<zaf::DataNode> ConvertDataNodeFromXmlString(const std::string& string) {

    auto stream = zaf::CreateMemoryStream(string.data(), string.size());

    zaf::XmlReader xml_reader;
    std::error_code error_code;
    return xml_reader.Execute(stream, error_code);
}

std::size_t GetDataNodeFieldCount(const std::shared_ptr<zaf::DataNode>& data_node) {

    std::size_t count = 0;
    data_node->EnumerateFields([&count](const std::wstring& key, const zaf::DataNode& data_node) {
        ++count;
    });
    return count;
}

const zaf::DataNode* GetDataNodeField(const zaf::DataNode& data_node, const std::wstring& expected_key) {

    const zaf::DataNode* field_node = nullptr;
    data_node.EnumerateFields([&expected_key, &field_node](const std::wstring& key, const zaf::DataNode& data_node) {
        
        if (expected_key == key) {
            field_node = &data_node;
        }
    });
    return field_node;
}

const zaf::DataNode* GetDataNodeElement(const zaf::DataNode& data_node, std::size_t index) {

    std::size_t current_index = 0;
    const zaf::DataNode* element_node = nullptr;
    data_node.EnumerateElements([index, &current_index, &element_node](const zaf::DataNode& data_node) {
    
        if (current_index == index) {
            element_node = &data_node;
        }

        ++current_index;
    });

    return element_node;
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
    ASSERT_EQ(GetDataNodeFieldCount(data_node), 0);

    data_node = ConvertDataNodeFromXmlString("<object></object>");
    ASSERT_NE(data_node, nullptr);
    ASSERT_TRUE(data_node->IsObject());
    ASSERT_EQ(GetDataNodeFieldCount(data_node), 0);

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

    auto field_node = GetDataNodeField(*data_node, L"S");
    ASSERT_NE(field_node, nullptr);
    ASSERT_TRUE(field_node->IsString());
    ASSERT_EQ(field_node->GetString(), L"zaf");

    field_node = GetDataNodeField(*data_node, L"B");
    ASSERT_NE(field_node, nullptr);
    ASSERT_TRUE(field_node->IsBoolean());
    ASSERT_EQ(field_node->GetBoolean(), true);

    field_node = GetDataNodeField(*data_node, L"N");
    ASSERT_NE(field_node, nullptr);
    ASSERT_TRUE(field_node->IsNumber());
    ASSERT_EQ(field_node->GetInt32(), 2);

    field_node = GetDataNodeField(*data_node, L"O");
    ASSERT_NE(field_node, nullptr);
    ASSERT_TRUE(field_node->IsObject());

    auto field_field_node = GetDataNodeField(*field_node, L"SS");
    ASSERT_NE(field_field_node, nullptr);
    ASSERT_TRUE(field_field_node->IsString());
    ASSERT_EQ(field_field_node->GetString(), L"ffff");

    field_node = GetDataNodeField(*data_node, L"A");
    ASSERT_NE(field_node, nullptr);
    ASSERT_TRUE(field_node->IsArray());

    auto field_element_node = GetDataNodeElement(*field_node, 0);
    ASSERT_NE(field_element_node, nullptr);
    ASSERT_EQ(field_element_node->GetString(), L"ara");
}