#include <gtest/gtest.h>
#include <zaf/graphic/size.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>

TEST(Size, Deserialize) {

    auto data_node = zaf::DataNode::CreateObject();
    data_node->AddChild(zaf::property::Width, zaf::DataNode::CreateNumber(99));
    data_node->AddChild(zaf::property::Height, zaf::DataNode::CreateNumber(130));

    zaf::Size size;
    bool is_succeeded = size.Deserialize(*data_node);
    ASSERT_TRUE(is_succeeded);
    ASSERT_EQ(size.width, 99);
    ASSERT_EQ(size.height, 130);
}