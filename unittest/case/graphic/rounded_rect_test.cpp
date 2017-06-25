#include <gtest/gtest.h>
#include <zaf/graphic/rounded_rect.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>

TEST(RoundedRect, Deserialize) {

    auto position_node = zaf::DataNode::CreateObject();
    position_node->AddChild(zaf::property::X, zaf::DataNode::CreateNumber(2));
    position_node->AddChild(zaf::property::Y, zaf::DataNode::CreateNumber(3));

    auto size_node = zaf::DataNode::CreateObject();
    size_node->AddChild(zaf::property::Width, zaf::DataNode::CreateNumber(4));
    size_node->AddChild(zaf::property::Height, zaf::DataNode::CreateNumber(5));

    auto rect_node = zaf::DataNode::CreateObject();
    rect_node->AddChild(zaf::property::Position, position_node);
    rect_node->AddChild(zaf::property::Size, size_node);

    auto data_node = zaf::DataNode::CreateObject();
    data_node->AddChild(zaf::property::Rect, rect_node);
    data_node->AddChild(zaf::property::XRadius, zaf::DataNode::CreateNumber(6));
    data_node->AddChild(zaf::property::YRadius, zaf::DataNode::CreateNumber(7));

    zaf::RoundedRect rounded_rect;
    bool is_succeeded = rounded_rect.Deserialize(*data_node);
    ASSERT_TRUE(is_succeeded);
    ASSERT_EQ(rounded_rect.x_radius, 6);
    ASSERT_EQ(rounded_rect.y_radius, 7);
    ASSERT_EQ(rounded_rect.rect.position.x, 2);
    ASSERT_EQ(rounded_rect.rect.position.y, 3);
    ASSERT_EQ(rounded_rect.rect.size.width, 4);
    ASSERT_EQ(rounded_rect.rect.size.height, 5);
}