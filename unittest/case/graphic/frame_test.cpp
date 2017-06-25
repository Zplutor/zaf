#include <gtest/gtest.h>
#include <zaf/graphic/frame.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>

TEST(Frame, Deserialize) {

    auto data_node = zaf::DataNode::CreateObject();
    data_node->AddChild(zaf::property::Left, zaf::DataNode::CreateNumber(71));
    data_node->AddChild(zaf::property::Right, zaf::DataNode::CreateNumber(72));
    data_node->AddChild(zaf::property::Top, zaf::DataNode::CreateNumber(73));
    data_node->AddChild(zaf::property::Bottom, zaf::DataNode::CreateNumber(74));

    zaf::Frame frame;
    bool is_succeeded = frame.Deserialize(*data_node);
    ASSERT_TRUE(is_succeeded);
    ASSERT_EQ(frame.left, 71);
    ASSERT_EQ(frame.right, 72);
    ASSERT_EQ(frame.top, 73);
    ASSERT_EQ(frame.bottom, 74);

    data_node = zaf::DataNode::CreateObject();
    data_node->AddChild(zaf::property::All, zaf::DataNode::CreateNumber(77));
    is_succeeded = frame.Deserialize(*data_node);
    ASSERT_TRUE(is_succeeded);
    ASSERT_EQ(frame.left, 77);
    ASSERT_EQ(frame.right, 77);
    ASSERT_EQ(frame.top, 77);
    ASSERT_EQ(frame.bottom, 77);
}