#include <gtest/gtest.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/json/json_writer.h>

namespace {
    
std::string ConvertDataNodeToJsonString(const std::shared_ptr<zaf::DataNode>& data_node) {

    zaf::JsonWriter writer;
    return writer.Write(data_node);
}

}

TEST(JsonWriter, WriteStringNode) {

    auto test = [](const std::wstring& string, const std::string& expected) {
        auto json_string = ConvertDataNodeToJsonString(zaf::DataNode::CreateString(string));
        return json_string == expected;
    };

    ASSERT_TRUE(test(L"", "\"\""));
    ASSERT_TRUE(test(L"zaf", "\"zaf\""));
}


TEST(JsonWriter, WriteBooleanNode) {

    auto test = [](bool value, const std::string& expected) {
        auto json_string = ConvertDataNodeToJsonString(zaf::DataNode::CreateBoolean(value));
        return json_string == expected;
    };

    ASSERT_TRUE(test(true, "true"));
    ASSERT_TRUE(test(false, "false"));
}


TEST(JsonWriter, WriteNumberNode) {

    auto test = [](zaf::Number value, const std::string& expected) {
        auto json_string = ConvertDataNodeToJsonString(zaf::DataNode::CreateNumber(value));
        return json_string == expected;
    };

    ASSERT_TRUE(test(13, "13"));
    ASSERT_TRUE(test(-13, "-13"));
}


TEST(JsonWriter, WriteObjectNode) {

    auto data_node = zaf::DataNode::CreateObject();
    auto json_string = ConvertDataNodeToJsonString(data_node);
    ASSERT_EQ(json_string, "{}");

    data_node->AddChild(L"String", zaf::DataNode::CreateString(L"zaf"));
    data_node->AddChild(L"Boolean", zaf::DataNode::CreateBoolean(true));
    data_node->AddChild(L"Number", zaf::DataNode::CreateNumber(22));
    data_node->AddChild(L"Object", zaf::DataNode::CreateObject());
    data_node->AddChild(L"Array", zaf::DataNode::CreateArray());
    json_string = ConvertDataNodeToJsonString(data_node);
    const char* expected = 
        "{"
        "\"String\":\"zaf\","
        "\"Boolean\":true,"
        "\"Number\":22,"
        "\"Object\":{},"
        "\"Array\":[]"
        "}";
    ASSERT_EQ(json_string, expected);
}


TEST(JsonWriter, WriteArrayNode) {

    auto data_node = zaf::DataNode::CreateArray();
    auto json_string = ConvertDataNodeToJsonString(data_node);
    ASSERT_EQ(json_string, "[]");

    data_node->AddChild(zaf::DataNode::CreateString(L"zaf"));
    data_node->AddChild(zaf::DataNode::CreateBoolean(true));
    data_node->AddChild(zaf::DataNode::CreateNumber(34));
    data_node->AddChild(zaf::DataNode::CreateObject());
    data_node->AddChild(zaf::DataNode::CreateArray());
    json_string = ConvertDataNodeToJsonString(data_node);
    const char* expected = 
        "["
        "\"zaf\","
        "true,"
        "34,"
        "{},"
        "[]"
        "]";
    ASSERT_EQ(json_string, expected);
}