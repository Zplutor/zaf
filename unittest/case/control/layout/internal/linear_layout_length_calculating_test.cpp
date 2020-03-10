#include <gtest/gtest.h>
#include <zaf/control/layout/internal/linear_layout_length_calculating.h>

namespace {

std::vector<zaf::internal::LinearLayoutLengthCalculatItem> CreateItems(
    const std::vector<std::pair<float, float>>& min_and_max) {

    std::vector<zaf::internal::LinearLayoutLengthCalculatItem> items;

    for (const auto& each_pair : min_and_max) {

        zaf::internal::LinearLayoutLengthCalculatItem each_item;
        each_item.minimum_length = each_pair.first;
        each_item.maximum_length = each_pair.second;
        items.push_back(each_item);
    }

    return items;
}


bool TestCalculating(
    float total_length,
    const std::vector<std::pair<float, float>>& mins_and_maxes,
    const std::vector<float>& expected_lengths) {

    auto items = CreateItems(mins_and_maxes);
    zaf::internal::CalculateArrayLayoutLengths(total_length, items);

    for (std::size_t index = 0; index < items.size(); ++index) {
        if (items[index].current_length != expected_lengths[index]) {
            return false;
        }
    }

    return true;
}

}


TEST(TestCalculateArrayLayoutLengths, NoItem) {

    std::vector<zaf::internal::LinearLayoutLengthCalculatItem> items;
    zaf::internal::CalculateArrayLayoutLengths(100, items);
    ASSERT_TRUE(items.empty());
}


TEST(TestCalculateArrayLayoutLengths, SingleItem) {

    auto items = CreateItems({{ 40.f, 100.f }});

    zaf::internal::CalculateArrayLayoutLengths(0, items);
    ASSERT_EQ(items[0].current_length, 40);

    zaf::internal::CalculateArrayLayoutLengths(20, items);
    ASSERT_EQ(items[0].current_length, 40);

    zaf::internal::CalculateArrayLayoutLengths(40, items);
    ASSERT_EQ(items[0].current_length, 40);

    zaf::internal::CalculateArrayLayoutLengths(70, items);
    ASSERT_EQ(items[0].current_length, 70);

    zaf::internal::CalculateArrayLayoutLengths(100, items);
    ASSERT_EQ(items[0].current_length, 100);

    zaf::internal::CalculateArrayLayoutLengths(200, items);
    ASSERT_EQ(items[0].current_length, 100);
}


TEST(TestCalculateArrayLayoutLengths, MultipleItems) {

    ASSERT_TRUE(TestCalculating(
        100,
        { { 0.f, 20.f }, { 0.f, 100.f } },
        { 20, 80 }));

    ASSERT_TRUE(TestCalculating(
        90,
        { { 0.f, 100.f }, { 0.f, 100.f }, { 0.f, 100.f } },
        { 30, 30, 30 }));

    ASSERT_TRUE(TestCalculating(
        100,
        { { 0.f, 100.f }, { 50.f, 100.f }, { 0.f, 100.f } },
        { 25, 50, 25 }));

    ASSERT_TRUE(TestCalculating(
        90,
        { { 0.f, 10.f }, { 0.f, 10.f }, { 0.f, 10.f } },
        { 10, 10, 10 }));

    ASSERT_TRUE(TestCalculating(
        90,
        { { 0.f, 50.f }, { 0.f, 50.f }, { 0.f, 50.f } },
        { 30.f, 30.f, 30.f }));

    ASSERT_TRUE(TestCalculating(
        100,
        { { 0.f, 20.f }, { 0.f, 100.f }, { 40.f, 100.f } },
        { 20.f, 40.f, 40.f }));

    ASSERT_TRUE(TestCalculating(100,
        { { 0.f, 100.f }, { 0.f, 20.f }, { 0.f, 20.f } },
        { 60.f, 20.f, 20.f }));
}