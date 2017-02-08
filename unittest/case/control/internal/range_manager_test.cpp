#include <gtest/gtest.h>
#include <zaf/base/define.h>
#include <zaf/control/internal/range_manager.h>

using namespace zaf::internal;

static bool CheckRanges(
    const RangeManager& range_manager, 
    const std::initializer_list<std::pair<std::size_t, std::size_t>>& expected_ranges);

TEST(RangeManagerTest, AddRangeWithInvalidRange) {

    RangeManager range_manager;

    auto test = [&range_manager](std::size_t position, std::size_t length) {
        range_manager.AddRange(position, length);
        return range_manager.GetRangeCount() == 0;
    };

    ASSERT_TRUE(test(zaf::InvalidIndex, 0));
    ASSERT_TRUE(test(zaf::InvalidIndex, 1));
    ASSERT_TRUE(test(zaf::InvalidIndex, 2));
    ASSERT_TRUE(test(0, 0));
    ASSERT_TRUE(test(1, 0));
    ASSERT_TRUE(test(2, 0));
}


TEST(RangeManagerTest, AddRangeToEmpty) {

    auto test = [](std::size_t position, std::size_t length) {

        RangeManager range_manager;
        range_manager.AddRange(position, length);

        if (range_manager.GetRangeCount() != 1) {
            return false;
        }

        auto range = range_manager.GetRangeAtIndex(0);
        return (range.first == position) && (range.second == length);
    };

    ASSERT_TRUE(test(0, 1));
    ASSERT_TRUE(test(0, 2));
    ASSERT_TRUE(test(1, 1));
    ASSERT_TRUE(test(1, 2));
}


TEST(RangeManagerTest, AddRangeToNonEmpty) {

    auto test = [](
        std::size_t position,
        std::size_t length, 
        const std::initializer_list<std::pair<std::size_t, std::size_t>>& expected_ranges) {

        RangeManager range_manager;
        range_manager.AddRange(0, 2);
        range_manager.AddRange(5, 1);
        range_manager.AddRange(10, 3);
        range_manager.AddRange(15, 6);

        range_manager.AddRange(position, length);

        return CheckRanges(range_manager, expected_ranges);
    };

    ASSERT_TRUE(test(2,3, { { 0, 2 }, { 2, 3 }, { 5, 1 }, { 10, 3 }, { 15, 6 } }));
    ASSERT_TRUE(test(30, 5, { { 0, 2 }, { 5, 1 }, { 10, 3 }, { 15, 6 }, { 30, 5 } }));
    ASSERT_TRUE(test(0, 3, { { 0, 3 }, { 5, 1 }, { 10, 3 }, { 15, 6 } }));
    ASSERT_TRUE(test(0, 50, { { 0, 50 } }));
    ASSERT_TRUE(test(11, 1, { { 0, 2 }, { 5, 1 }, { 10, 1 }, { 11, 1 }, { 12, 1 }, { 15, 6 } }));
    ASSERT_TRUE(test(17, 3, { { 0, 2 }, { 5, 1 }, { 10, 3 }, { 15, 2 }, { 17, 3 }, { 20, 1 } }));
    ASSERT_TRUE(test(14, 3, { { 0, 2 }, { 5, 1 }, { 10, 3 }, { 14, 3 }, { 17, 4 } }));
    ASSERT_TRUE(test(10, 6, { { 0, 2 }, { 5, 1 }, { 10, 6 }, { 16, 5 } }));
    ASSERT_TRUE(test(15, 3, { { 0, 2 }, { 5, 1 }, { 10, 3 }, { 15, 3 }, { 18, 3 } }));
    ASSERT_TRUE(test(19, 3, { { 0, 2 }, { 5, 1 }, { 10, 3 }, { 15, 4 }, { 19, 3 } }));
    ASSERT_TRUE(test(19, 2, { { 0, 2 }, { 5, 1 }, { 10, 3 }, { 15, 4 }, { 19, 2 } }));
}


TEST(RangeManagerTest, RemoveRangeWithInvalidRange) {

    RangeManager range_manager;
    range_manager.AddRange(0, 10);

    auto test = [&range_manager](std::size_t position, std::size_t length) {

        range_manager.RemoveRange(position, length);
        return range_manager.GetRangeCount() == 1;
    };

    ASSERT_TRUE(test(zaf::InvalidIndex, 2));
    ASSERT_TRUE(test(zaf::InvalidIndex, 0));
    ASSERT_TRUE(test(0, 0));
}


TEST(RangeManagerTest, RemoveRangeFromEmpty) {

    auto test = [](std::size_t position, std::size_t length) {

        RangeManager range_manager;
        range_manager.RemoveRange(position, length);
        return range_manager.GetRangeCount() == 0;
    };

    ASSERT_TRUE(test(0, 1));
    ASSERT_TRUE(test(5, 3));
    ASSERT_TRUE(test(20, 2));
}


TEST(RangeManagerTest, RemoveRangeFromNonEmpty) {
 
    auto test = [](
        std::size_t position, 
        std::size_t length, 
        const std::initializer_list<std::pair<std::size_t, std::size_t>>& expected_ranges) {

        RangeManager range_manager;
        range_manager.AddRange(0, 2);
        range_manager.AddRange(5, 1);
        range_manager.AddRange(10, 3);
        range_manager.AddRange(15, 6);

        range_manager.RemoveRange(position, length);

        return CheckRanges(range_manager, expected_ranges);
    };

    ASSERT_TRUE(test(2, 2, { { 0, 2 }, { 5, 1 }, { 10, 3 }, { 15, 6 } }));
    ASSERT_TRUE(test(30, 5, { { 0, 2 }, { 5, 1 }, { 10, 3 }, { 15, 6 } }));

    ASSERT_TRUE(test(0, 3, { { 5, 1 }, { 10, 3 }, { 15, 6 } }));
    ASSERT_TRUE(test(4, 3, { { 0, 2 }, { 10, 3 }, { 15, 6 } }));
    ASSERT_TRUE(test(0, 30, { }));

    ASSERT_TRUE(test(11, 1, { { 0, 2 }, { 5, 1 }, { 10, 1 }, { 12, 1 }, { 15, 6 } }));
    ASSERT_TRUE(test(17, 3, { { 0, 2 }, { 5, 1 }, { 10, 3 }, { 15, 2 }, { 20, 1 } }));

    ASSERT_TRUE(test(0, 1, { { 1, 1 }, { 5, 1 }, { 10, 3 }, { 15, 6 } }));
    ASSERT_TRUE(test(1, 1, { { 0, 1 }, { 5, 1 }, { 10, 3 }, { 15, 6 } }));
    ASSERT_TRUE(test(13, 3, { { 0, 2 }, { 5, 1 }, { 10, 3 }, { 16, 5 } }));
    ASSERT_TRUE(test(19, 5, { { 0, 2 }, { 5, 1 }, { 10, 3 }, { 15, 4 } }));
}


TEST(RangeManagerTest, ExpandRangesWithInvalidRange) {

    RangeManager range_manager;
    range_manager.AddRange(0, 10);

    auto test = [&range_manager](std::size_t position, std::size_t length) {

        range_manager.ExpandRanges(position, length);

        if (range_manager.GetRangeCount() != 1) {
            return false;
        }

        auto range = range_manager.GetRangeAtIndex(0);
        return (range.first == 0) && (range.second == 10);
    };

    ASSERT_TRUE(test(zaf::InvalidIndex, 1));
    ASSERT_TRUE(test(zaf::InvalidIndex, 0));
    ASSERT_TRUE(test(3, 0));
}


TEST(RangeManagerTest, ExpandRanges) {

    auto test = [](
        std::size_t position,
        std::size_t length, 
        const std::initializer_list<std::pair<std::size_t, std::size_t>>& expected_ranges) {

        RangeManager range_manager;
        range_manager.AddRange(5, 4);
        range_manager.AddRange(20, 4);

        range_manager.ExpandRanges(position, length);

        return CheckRanges(range_manager, expected_ranges);
    };

    ASSERT_TRUE(test(1, 2, { { 7, 4 }, { 22, 4 } }));
    ASSERT_TRUE(test(10, 3, { { 5, 4 }, { 23, 4 } }));

    ASSERT_TRUE(test(6, 2, { { 5, 1 }, { 8, 3 }, { 22, 4 } }));
    ASSERT_TRUE(test(22, 2, { { 5, 4 }, { 20, 2 }, { 24, 2 } }));

    ASSERT_TRUE(test(25, 3, { { 5, 4 }, { 20, 4 } }));
}



TEST(RangeManagerTest, NarrowRangesWithInvalidRange) {

    RangeManager range_manager;
    range_manager.AddRange(0, 10);

    auto test = [&range_manager](std::size_t position, std::size_t length) {

        range_manager.NarrowRanges(position, length);

        if (range_manager.GetRangeCount() != 1) {
            return false;
        }

        auto range = range_manager.GetRangeAtIndex(0);
        return (range.first == 0) && (range.second == 10);
    };

    ASSERT_TRUE(test(zaf::InvalidIndex, 1));
    ASSERT_TRUE(test(zaf::InvalidIndex, 0));
    ASSERT_TRUE(test(3, 0));
}


TEST(RangeManagerTest, NarrowRanges) {

    auto test = [](
        std::size_t position,
        std::size_t length,
        const std::initializer_list<std::pair<std::size_t, std::size_t>>& expected_ranges) {

        RangeManager range_manager;
        range_manager.AddRange(5, 4);
        range_manager.AddRange(20, 4);

        range_manager.NarrowRanges(position, length);

        return CheckRanges(range_manager, expected_ranges);
    };

    ASSERT_TRUE(test(0, 2, { { 3, 4 }, { 18, 4 } }));
    ASSERT_TRUE(test(10, 3, { { 5, 4 }, { 17, 4 } }));

    ASSERT_TRUE(test(3, 10, { { 10, 4 } }));
    ASSERT_TRUE(test(3, 30, { }));

    ASSERT_TRUE(test(6, 2, { { 5, 2 }, { 18, 4 } }));
    ASSERT_TRUE(test(21, 2, { { 5, 4 }, { 20, 2 } }));

    ASSERT_TRUE(test(3, 3, { { 3, 3 }, { 17, 4 } }));
    ASSERT_TRUE(test(17, 5, { { 5, 4 }, { 17, 2 } }));

    ASSERT_TRUE(test(7, 3, { { 5, 2 }, { 17, 4 } }));
    ASSERT_TRUE(test(23, 5, { { 5, 4 }, { 20, 3 } }));

    ASSERT_TRUE(test(30, 3, { { 5, 4 }, { 20, 4 } }));
}


static bool CheckRanges(
    const RangeManager& range_manager,
    const std::initializer_list<std::pair<std::size_t, std::size_t>>& expected_ranges) {

    if (range_manager.GetRangeCount() != expected_ranges.size()) {
        return false;
    }

    std::size_t index = 0;
    for (const auto& each_range : expected_ranges) {

        auto actual_range = range_manager.GetRangeAtIndex(index);

        if ((actual_range.first != each_range.first) ||
            (actual_range.second != each_range.second)) {
            return false;
        }

        ++index;
    }

    return true;
}