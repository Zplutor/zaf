#include <gtest/gtest.h>
#include <zaf/internal/ranged_value_store.h>

using namespace zaf;
using namespace zaf::internal;

namespace {

bool CheckRanges(
    const RangedValueStore& range_manager,
    const std::initializer_list<std::pair<std::size_t, std::size_t>>& expected_ranges) {

    if (range_manager.Items().size() != expected_ranges.size()) {
        return false;
    }

    std::size_t index = 0;
    for (const auto& each_range : expected_ranges) {

        auto actual_range = range_manager.Items()[index].Range();

        if ((actual_range.index != each_range.first) ||
            (actual_range.length != each_range.second)) {
            return false;
        }

        ++index;
    }

    return true;
}

}


TEST(RangedValueStoreTest, AddRangeWithInvalidRange) {

    RangedValueStore range_manager;

    auto test = [&range_manager](std::size_t position, std::size_t length) {
        bool is_succeeded = range_manager.AddRange(position, length);
        return (!is_succeeded) && range_manager.Items().empty();
    };

    ASSERT_TRUE(test(0, 0));
    ASSERT_TRUE(test(1, 0));
    ASSERT_TRUE(test(2, 0));
}


TEST(RangedValueStoreTest, AddRangeToEmpty) {

    auto test = [](std::size_t position, std::size_t length) {

        RangedValueStore range_manager;
        range_manager.AddRange(position, length);

        if (range_manager.Items().size() != 1) {
            return false;
        }

        auto range = range_manager.Items()[0].Range();
        return (range.index == position) && (range.length == length);
    };

    ASSERT_TRUE(test(0, 1));
    ASSERT_TRUE(test(0, 2));
    ASSERT_TRUE(test(1, 1));
    ASSERT_TRUE(test(1, 2));
}


TEST(RangedValueStoreTest, AddRangeToNonEmpty) {

    auto test = [](
        std::size_t position,
        std::size_t length, 
        const std::initializer_list<std::pair<std::size_t, std::size_t>>& expected_ranges) {

        RangedValueStore range_manager;
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


TEST(RangedValueStoreTest, RemoveRangeWithInvalidRange) {

    RangedValueStore range_manager;
    range_manager.AddRange(0, 10);

    auto test = [&range_manager](std::size_t position, std::size_t length) {

        range_manager.RemoveRange(position, length);
        return range_manager.Items().size() == 1;
    };

    ASSERT_TRUE(test(0, 0));
    ASSERT_TRUE(test(1, 0));
    ASSERT_TRUE(test(2, 0));
}


TEST(RangedValueStoreTest, RemoveRangeFromEmpty) {

    auto test = [](std::size_t position, std::size_t length) {

        RangedValueStore range_manager;
        range_manager.RemoveRange(position, length);
        return range_manager.Items().empty();
    };

    ASSERT_TRUE(test(0, 1));
    ASSERT_TRUE(test(5, 3));
    ASSERT_TRUE(test(20, 2));
}


TEST(RangedValueStoreTest, RemoveRangeFromNonEmpty) {
 
    auto test = [](
        std::size_t position, 
        std::size_t length, 
        const std::initializer_list<std::pair<std::size_t, std::size_t>>& expected_ranges) {

        RangedValueStore range_manager;
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


TEST(RangedValueStoreTest, InsertSpanWithInvalidRange) {

    RangedValueStore range_manager;
    range_manager.AddRange(0, 10);

    auto test = [&range_manager](std::size_t position, std::size_t length) {

        range_manager.InsertSpan(zaf::Range{ position, length });

        if (range_manager.Items().size() != 1) {
            return false;
        }

        auto range = range_manager.Items()[0].Range();
        return (range.index == 0) && (range.length == 10);
    };

    ASSERT_TRUE(test(0, 0));
    ASSERT_TRUE(test(1, 0));
    ASSERT_TRUE(test(2, 0));
    ASSERT_TRUE(test(3, 0));
}


TEST(RangedValueStoreTest, InsertSpan) {

    auto test = [](
        std::size_t position,
        std::size_t length, 
        const std::initializer_list<std::pair<std::size_t, std::size_t>>& expected_ranges) {

        RangedValueStore range_manager;
        range_manager.AddRange(5, 4);
        range_manager.AddRange(20, 4);

        range_manager.InsertSpan(zaf::Range{ position, length });

        return CheckRanges(range_manager, expected_ranges);
    };

    ASSERT_TRUE(test(1, 2, { { 7, 4 }, { 22, 4 } }));
    ASSERT_TRUE(test(10, 3, { { 5, 4 }, { 23, 4 } }));

    ASSERT_TRUE(test(6, 2, { { 5, 1 }, { 8, 3 }, { 22, 4 } }));
    ASSERT_TRUE(test(22, 2, { { 5, 4 }, { 20, 2 }, { 24, 2 } }));

    ASSERT_TRUE(test(25, 3, { { 5, 4 }, { 20, 4 } }));
}



TEST(RangedValueStoreTest, EraseSpanWithInvalidRange) {

    RangedValueStore range_manager;
    range_manager.AddRange(0, 10);

    auto test = [&range_manager](std::size_t position, std::size_t length) {

        range_manager.EraseSpan(zaf::Range{ position, length });

        if (range_manager.Items().size() != 1) {
            return false;
        }

        auto range = range_manager.Items()[0].Range();
        return (range.index == 0) && (range.length == 10);
    };

    ASSERT_TRUE(test(0, 0));
    ASSERT_TRUE(test(1, 0));
    ASSERT_TRUE(test(2, 0));
    ASSERT_TRUE(test(3, 0));
}


TEST(RangedValueStoreTest, EraseSpan) {

    auto test = [](
        std::size_t position,
        std::size_t length,
        const std::initializer_list<std::pair<std::size_t, std::size_t>>& expected_ranges) {

        RangedValueStore range_manager;
        range_manager.AddRange(5, 4);
        range_manager.AddRange(20, 4);

        range_manager.EraseSpan(zaf::Range{ position, length });

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


TEST(RangedValueStoreTest, FindItemContainsIndex) {

    RangedValueStore store;
    ASSERT_EQ(store.FindItemContainsIndex(0), nullptr);
    ASSERT_EQ(store.FindItemContainsIndex(1), nullptr);

    store.AddRange(Range{ 1, 2 }, std::string{ "range1" });
    store.AddRange(Range{ 5, 1 }, std::string{ "range2" });
    store.AddRange(Range{ 10, 5 }, std::string{ "range2" });

    //Invalid indexes.
    auto item = store.FindItemContainsIndex(0);
    ASSERT_EQ(item, nullptr);

    item = store.FindItemContainsIndex(4);
    ASSERT_EQ(item, nullptr);

    item = store.FindItemContainsIndex(9);
    ASSERT_EQ(item, nullptr);

    item = store.FindItemContainsIndex(16);
    ASSERT_EQ(item, nullptr);

    //The first range
    item = store.FindItemContainsIndex(1);
    ASSERT_NE(item, nullptr);
    ASSERT_EQ(item->Range(), Range(1, 2));

    item = store.FindItemContainsIndex(2);
    ASSERT_NE(item, nullptr);
    ASSERT_EQ(item->Range(), Range(1, 2));

    item = store.FindItemContainsIndex(3);
    ASSERT_EQ(item, nullptr);

    //The second range
    item = store.FindItemContainsIndex(5);
    ASSERT_NE(item, nullptr);
    ASSERT_EQ(item->Range(), Range(5, 1));

    item = store.FindItemContainsIndex(6);
    ASSERT_EQ(item, nullptr);

    //The third range
    item = store.FindItemContainsIndex(10);
    ASSERT_NE(item, nullptr);
    ASSERT_EQ(item->Range(), Range(10, 5));

    item = store.FindItemContainsIndex(13);
    ASSERT_NE(item, nullptr);
    ASSERT_EQ(item->Range(), Range(10, 5));

    item = store.FindItemContainsIndex(15);
    ASSERT_EQ(item, nullptr);
}


TEST(RangedValueStoreTest, FindItemContainsIndexConst) {

    RangedValueStore store;
    store.AddRange(Range{ 0, 5 }, {});

    const RangedValueStore& const_store = store;
    auto item = const_store.FindItemContainsIndex(1);
    ASSERT_NE(item, nullptr);
    ASSERT_EQ(item->Range(), Range(0, 5));
}


TEST(RangedValueStoreTest, RangedFor) {

    std::vector<Range> test_ranges{
        Range{ 0, 1 },
        Range{ 1, 1 },
        Range{ 2, 1 },
    };

    RangedValueStore store;
    for (const auto& each_range : test_ranges) {
        store.AddRange(each_range, {});
    }

    std::vector<Range> ranges;
    for (auto& item : store) {
        static_assert(!std::is_const_v<std::remove_reference_t<decltype(item)>>);
        ranges.push_back(item.Range());
    }
    ASSERT_EQ(ranges, test_ranges);

    ranges.clear();
    for (auto& item : (const RangedValueStore&)store) {
        static_assert(std::is_const_v<std::remove_reference_t<decltype(item)>>);
        ranges.push_back(item.Range());
    }
    ASSERT_EQ(ranges, test_ranges);
}