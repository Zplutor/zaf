#include <gtest/gtest.h>
#include <zaf/graphic/ellipse.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>

TEST(Ellipse, Deserialize) {

    auto position_node = zaf::DataNode::CreateObject();
    position_node->AddChild(zaf::property::X, zaf::DataNode::CreateNumber(23));
    position_node->AddChild(zaf::property::Y, zaf::DataNode::CreateNumber(24));

    auto data_node = zaf::DataNode::CreateObject();
    data_node->AddChild(zaf::property::Position, position_node);
    data_node->AddChild(zaf::property::XRadius, zaf::DataNode::CreateNumber(25));
    data_node->AddChild(zaf::property::YRadius, zaf::DataNode::CreateNumber(26));

    zaf::Ellipse ellipse;
    bool is_succeeded = ellipse.Deserialize(*data_node);
    ASSERT_TRUE(is_succeeded);
    ASSERT_EQ(ellipse.x_radius, 25);
    ASSERT_EQ(ellipse.y_radius, 26);
    ASSERT_EQ(ellipse.position.x, 23);
    ASSERT_EQ(ellipse.position.y, 24);
}