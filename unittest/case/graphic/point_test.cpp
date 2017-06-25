#include <gtest/gtest.h>
#include <zaf/graphic/point.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>

TEST(Point, Deserialize) {

    auto data_node = zaf::DataNode::CreateObject();
    data_node->AddChild(zaf::property::X, zaf::DataNode::CreateNumber(9));
    data_node->AddChild(zaf::property::Y, zaf::DataNode::CreateNumber(10));

    zaf::Point point;
    bool is_succeeded = point.Deserialize(*data_node);
    ASSERT_TRUE(is_succeeded);
    ASSERT_EQ(point.x, 9);
    ASSERT_EQ(point.y, 10);
}