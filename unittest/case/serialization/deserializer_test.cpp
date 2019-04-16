#include <gtest/gtest.h>
#include <zaf/graphic/color.h>
#include <zaf/serialization/deserializer.h>

class CustomObject {
public:
    int value = 0;
};

template<>
class zaf::Deserializer<CustomObject> {
public:
    static bool Deserialize(const DataNode& data_node, CustomObject& object) {
        object.value = data_node.GetInt32();
        return true;
    }
};

TEST(Deserializer, Arithmetic) {

    auto data_node = zaf::DataNode::CreateNumber(14);
    bool is_succeeded = false;

    {
        std::int32_t number = 0;
        is_succeeded = zaf::GetDeserializer<std::int32_t>::Type::Deserialize(*data_node, number);
        ASSERT_TRUE(is_succeeded);
        ASSERT_EQ(number, 14);
    }

    {
        std::int64_t number = 0;
        is_succeeded = zaf::GetDeserializer<std::int64_t>::Type::Deserialize(*data_node, number);
        ASSERT_TRUE(is_succeeded);
        ASSERT_EQ(number, 14);
    }

    {
        float number = 0;
        is_succeeded = zaf::GetDeserializer<float>::Type::Deserialize(*data_node, number);
        ASSERT_TRUE(is_succeeded);
        ASSERT_EQ(number, 14.f);
    }
}


TEST(Deserializer, Number) {

    auto data_node = zaf::DataNode::CreateNumber(15);
    zaf::Number number;
    bool is_succeeded = zaf::GetDeserializer<zaf::Number>::Type::Deserialize(*data_node, number);
    ASSERT_TRUE(is_succeeded);
    ASSERT_EQ(number.GetValue<int>(), 15);
}


TEST(Deserializer, String) {

    auto data_node = zaf::DataNode::CreateString(L"deserializer");
    std::wstring string;
    bool is_succeeded = zaf::GetDeserializer<std::wstring>::Type::Deserialize(*data_node, string);
    ASSERT_TRUE(is_succeeded);
    ASSERT_EQ(string, L"deserializer");
}


TEST(Deserializer, Boolean) {

    auto data_node = zaf::DataNode::CreateBoolean(true);
    bool boolean = false;
    bool is_succeeded = zaf::GetDeserializer<bool>::Type::Deserialize(*data_node, boolean);
    ASSERT_TRUE(is_succeeded);
    ASSERT_EQ(boolean, true);
}


TEST(Deserializer, SerializableObject) {

    {
        auto data_node = zaf::DataNode::CreateObject();
        data_node->AddChild(L"Type", zaf::DataNode::CreateString(L"Color"));
        data_node->AddChild(L"A", zaf::DataNode::CreateNumber(0.03f));

        zaf::Color color;
        bool is_succeeded = zaf::GetDeserializer<zaf::Color>::Type::Deserialize(*data_node, color);
        ASSERT_TRUE(is_succeeded);
        ASSERT_EQ(color.a, 0.03f);
    }

    {
        auto data_node = zaf::DataNode::CreateObject();
        data_node->AddChild(L"Type", zaf::DataNode::CreateString(L"Unknown"));

        zaf::Color color;
        bool is_succeeded = zaf::GetDeserializer<zaf::Color>::Type::Deserialize(*data_node, color);
        ASSERT_FALSE(is_succeeded);
    }
}


TEST(Deserializer, CustomType) {

    auto data_node = zaf::DataNode::CreateNumber(11);
    CustomObject custom_object;
    bool is_succeeded = zaf::GetDeserializer<CustomObject>::Type::Deserialize(*data_node, custom_object);
    ASSERT_TRUE(is_succeeded);
    ASSERT_EQ(custom_object.value, 11);
}