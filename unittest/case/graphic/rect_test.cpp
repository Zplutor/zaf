#include <gtest/gtest.h>
#include <zaf/graphic/rect.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>

TEST(Rect, Contain) {

    ASSERT_FALSE(zaf::Rect(0, 0, 0, 0).Contain(zaf::Point(0, 0)));
    ASSERT_FALSE(zaf::Rect(0, 0, 0, 0).Contain(zaf::Point(1, 1)));
    ASSERT_FALSE(zaf::Rect(0, 0, 0, 0).Contain(zaf::Point(-1, -1)));

    ASSERT_TRUE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(0, 0)));
    ASSERT_TRUE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(5, 0)));
    ASSERT_TRUE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(0, 5)));

    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(10, 0)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(0, 10)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(10, 5)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(5, 10)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(10, 10)));

    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(-1, 0)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(0, -1)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(-1, -1)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(5, -5)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(-5, 5)));
    ASSERT_FALSE(zaf::Rect(0, 0, 10, 10).Contain(zaf::Point(11, 11)));
}


TEST(Rect, Intersect) {

    auto test = [](const zaf::Rect& intersected_rect, const zaf::Rect& expected_rect) {
        zaf::Rect primary_rect(0, 0, 10, 10);
        primary_rect.Intersect(intersected_rect);
        return primary_rect == expected_rect;
    };

    ASSERT_TRUE(test(zaf::Rect(-5, -5, 3, 3), zaf::Rect()));
    ASSERT_TRUE(test(zaf::Rect(-5, -5, 5, 5), zaf::Rect()));
    ASSERT_TRUE(test(zaf::Rect(0, -5, 5, 5), zaf::Rect()));
    ASSERT_TRUE(test(zaf::Rect(-5, 0, 5, 5), zaf::Rect()));
    ASSERT_TRUE(test(zaf::Rect(11, 11, 3, 2), zaf::Rect()));
    ASSERT_TRUE(test(zaf::Rect(10, 0, 3, 2), zaf::Rect()));
    ASSERT_TRUE(test(zaf::Rect(0, 10, 3, 2), zaf::Rect()));

    ASSERT_TRUE(test(zaf::Rect(0, 0, 3, 2), zaf::Rect(0, 0, 3, 2)));
    ASSERT_TRUE(test(zaf::Rect(4, 2, 6, 7), zaf::Rect(4, 2, 6, 7)));
    ASSERT_TRUE(test(zaf::Rect(-1, -1, 12, 12), zaf::Rect(0, 0, 10, 10)));

    ASSERT_TRUE(test(zaf::Rect(-3, -3, 6, 6), zaf::Rect(0, 0, 3, 3)));
    ASSERT_TRUE(test(zaf::Rect(5, 5, 8, 8), zaf::Rect(5, 5, 5, 5)));
}


TEST(Rect, InflateWithFrame) {

    auto test = [](const zaf::Frame& frame, const zaf::Rect& expected_rect) {
        zaf::Rect rect(0, 0, 5, 5);
        rect.Inflate(frame);
        return rect == expected_rect;
    };

    ASSERT_TRUE(test(zaf::Frame(0, 0, 0, 0), zaf::Rect(0, 0, 5, 5)));
    ASSERT_TRUE(test(zaf::Frame(1, 2, 3, 4), zaf::Rect(-1, -2, 9, 11)));
}


TEST(Rect, DeflateWithFrame) {

    auto test = [](const zaf::Frame& frame, const zaf::Rect& expected_rect) {
        zaf::Rect rect(0, 0, 10, 10);
        rect.Deflate(frame);
        return rect == expected_rect;
    };

    ASSERT_TRUE(test(zaf::Frame(0, 0, 0, 0), zaf::Rect(0, 0, 10, 10)));
    ASSERT_TRUE(test(zaf::Frame(1, 2, 3, 4), zaf::Rect(1, 2, 6, 4)));
}


TEST(Rect, Deserialize) {

    auto point_node = zaf::DataNode::CreateObject();
    point_node->AddChild(zaf::property::X, zaf::DataNode::CreateNumber(34));
    point_node->AddChild(zaf::property::Y, zaf::DataNode::CreateNumber(35));

    auto size_node = zaf::DataNode::CreateObject();
    size_node->AddChild(zaf::property::Width, zaf::DataNode::CreateNumber(36));
    size_node->AddChild(zaf::property::Height, zaf::DataNode::CreateNumber(37));

    auto data_node = zaf::DataNode::CreateObject();
    data_node->AddChild(zaf::property::Position, point_node);
    data_node->AddChild(zaf::property::Size, size_node);

    zaf::Rect rect;
    bool is_succeeded = rect.Deserialize(*data_node);
    ASSERT_TRUE(is_succeeded);
    ASSERT_EQ(rect.position.x, 34);
    ASSERT_EQ(rect.position.y, 35);
    ASSERT_EQ(rect.size.width, 36);
    ASSERT_EQ(rect.size.height, 37);
}