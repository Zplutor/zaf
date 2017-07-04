#include <gtest/gtest.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/json/json_reader.h>

namespace {

std::shared_ptr<zaf::DataNode> ReadDataNode(const std::string& json_string) {

    zaf::JsonReader reader;
    std::error_code error_code;
    return reader.Read(json_string, error_code);
}

}

TEST(JsonReader, ReadString) {

    auto test = [](const std::string& value, const std::wstring& expected) {
    
        auto data_node = ReadDataNode('"' + value + '"');
        if (data_node == nullptr) {
            return false;
        }

        if (! data_node->IsString()) {
            return false;
        }

        return data_node->GetString() == expected;
    };

    ASSERT_TRUE(test("", L""));
    ASSERT_TRUE(test("zaf", L"zaf"));
}


TEST(JsonReader, ReadBoolean) {

    auto test = [](const std::string& string_value, bool expected) {

        auto data_node = ReadDataNode(string_value);
        if (data_node == nullptr) {
            return false;
        }

        if (!data_node->IsBoolean()) {
            return false;
        }

        return data_node->GetBoolean() == expected;
    };

    ASSERT_TRUE(test("true", true));
    ASSERT_TRUE(test("false", false));
}


TEST(JsonReader, ReadNumber) {

    auto test = [](const std::string& string_value, const std::function<bool(zaf::Number)>& checker) {

        auto data_node = ReadDataNode(string_value);
        if (data_node == nullptr) {
            return false;
        }

        if (!data_node->IsNumber()) {
            return false;
        }

        return checker(data_node->GetNumber());
    };

    ASSERT_TRUE(test("0", [](zaf::Number number) { return number.GetValue<std::int64_t>() == 0; }));
    ASSERT_TRUE(test("-303", [](zaf::Number number) { return number.GetValue<std::int64_t>() == -303; }));
    ASSERT_TRUE(test("-1.39", [](zaf::Number number) { return number.GetValue<double>() == -1.39; }));
}


TEST(JsonReader, ReadObject) {

    const char* json_string = "{}";
    auto data_node = ReadDataNode(json_string);
    ASSERT_NE(data_node, nullptr);
    ASSERT_TRUE(data_node->IsObject());
    ASSERT_EQ(data_node->GetChildCount(), 0);

    json_string = 
        "{"
        "\"String\":\"zaf\","
        "\"Boolean\":true,"
        "\"Number\":3.14159,"
        "\"Object\":{},"
        "\"Array\":[],"
        "}";
    data_node = ReadDataNode(json_string);
    ASSERT_NE(data_node, nullptr);
    ASSERT_TRUE(data_node->IsObject());
    ASSERT_EQ(data_node->GetChildCount(), 5);

    auto child_node = data_node->GetChild(L"String");
    ASSERT_NE(child_node, nullptr);
    ASSERT_TRUE(child_node->IsString());
    ASSERT_EQ(child_node->GetString(), L"zaf");

    child_node = data_node->GetChild(L"Boolean");
    ASSERT_NE(child_node, nullptr);
    ASSERT_TRUE(child_node->IsBoolean());
    ASSERT_EQ(child_node->GetBoolean(), true);

    child_node = data_node->GetChild(L"Number");
    ASSERT_NE(child_node, nullptr);
    ASSERT_TRUE(child_node->IsNumber());
    ASSERT_EQ(child_node->GetNumber().GetValue<double>(), 3.14159);

    child_node = data_node->GetChild(L"Object");
    ASSERT_NE(child_node, nullptr);
    ASSERT_TRUE(child_node->IsObject());
    
    child_node = data_node->GetChild(L"Array");
    ASSERT_NE(child_node, nullptr);
    ASSERT_TRUE(child_node->IsArray());
}


TEST(JsonReader, ReadArray) {

    const char* json_string = "[]";
    auto data_node = ReadDataNode(json_string);
    ASSERT_NE(data_node, nullptr);
    ASSERT_TRUE(data_node->IsArray());
    ASSERT_EQ(data_node->GetChildCount(), 0);
    
    json_string =
        "["
        "\"zaf\","
        "true,"
        "3.14159,"
        "{},"
        "[],"
        "]";
    data_node = ReadDataNode(json_string);
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
    ASSERT_EQ(child_node->GetBoolean(), true);

    child_node = data_node->GetChild(2);
    ASSERT_NE(child_node, nullptr);
    ASSERT_TRUE(child_node->IsNumber());
    ASSERT_EQ(child_node->GetNumber().GetValue<double>(), 3.14159);

    child_node = data_node->GetChild(3);
    ASSERT_NE(child_node, nullptr);
    ASSERT_TRUE(child_node->IsObject());

    child_node = data_node->GetChild(4);
    ASSERT_NE(child_node, nullptr);
    ASSERT_TRUE(child_node->IsArray());
}


TEST(JsonReader, ReadWithSpaces) {

    const char* json_string = "  [   { \"Type\": \"Control\"},  {  } ]";
    auto data_node = ReadDataNode(json_string);
    ASSERT_NE(data_node, nullptr);
    ASSERT_EQ(data_node->GetChildCount(), 2);

    auto first = data_node->GetChild(0);
    ASSERT_NE(first, nullptr);
    ASSERT_TRUE(first->IsObject());

    auto second = data_node->GetChild(1);
    ASSERT_NE(second, nullptr);
    ASSERT_TRUE(second->IsObject());
}