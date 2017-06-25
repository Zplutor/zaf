#include <gtest/gtest.h>
#include <zaf/graphic/color.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>

TEST(Color, Deserialize) {

    auto data_node = zaf::DataNode::CreateObject();
    data_node->AddChild(zaf::property::A, zaf::DataNode::CreateNumber(0.2f));
    data_node->AddChild(zaf::property::R, zaf::DataNode::CreateNumber(0.24f));
    data_node->AddChild(zaf::property::G, zaf::DataNode::CreateNumber(0.36f));
    data_node->AddChild(zaf::property::B, zaf::DataNode::CreateNumber(0.57f));

    zaf::Color color;
    bool is_succeeded = color.Deserialize(*data_node);
    ASSERT_TRUE(is_succeeded);
    ASSERT_EQ(color.a, 0.2f);
    ASSERT_EQ(color.r, 0.24f);
    ASSERT_EQ(color.g, 0.36f);
    ASSERT_EQ(color.b, 0.57f);

    //ARGB notation
    data_node = zaf::DataNode::CreateObject();
    data_node->AddChild(zaf::property::ARGB, zaf::DataNode::CreateString(L"#AABBCCDD"));
    is_succeeded = color.Deserialize(*data_node);
    ASSERT_TRUE(is_succeeded);
    ASSERT_EQ(color.a, float(0xAA) / float(0xFF));
    ASSERT_EQ(color.r, float(0xBB) / float(0xFF));
    ASSERT_EQ(color.g, float(0xCC) / float(0xFF));
    ASSERT_EQ(color.b, float(0xDD) / float(0xFF));

    //RGB notation
    data_node = zaf::DataNode::CreateObject();
    data_node->AddChild(zaf::property::ARGB, zaf::DataNode::CreateString(L"#AABBCC"));
    is_succeeded = color.Deserialize(*data_node);
    ASSERT_TRUE(is_succeeded);
    ASSERT_EQ(color.a, 1);
    ASSERT_EQ(color.r, float(0xAA) / float(0xFF));
    ASSERT_EQ(color.g, float(0xBB) / float(0xFF));
    ASSERT_EQ(color.b, float(0xCC) / float(0xFF));
}