#include <gtest/gtest.h>
#include <zaf/base/range_map.h>

using namespace zaf;

namespace {

bool CheckRangeMapResult(
    const RangeMap<std::string>& map, 
    const RangeMap<std::string>::ItemList& expected) {

    if (map.Count() != expected.size()) {
        return false;
    }

    auto iterator1 = map.begin();
    auto iterator2 = expected.begin();
    while (iterator1 != map.end() && iterator2 != expected.end()) {

        if (iterator1->Range() != iterator2->Range()) {
            return false;
        }

        if (iterator1->Value() != iterator2->Value()) {
            return false;
        }

        ++iterator1;
        ++iterator2;
    }

    return true;
}

}

TEST(RangeMapTest, AddRangeToEmptyMap) {

    auto test = [](
        const Range& range, 
        const std::string& value,
        const RangeMap<std::string>::ItemList& expected) {
    
        RangeMap<std::string> map;
        map.AddRange(range, value);
        return CheckRangeMapResult(map, expected);
    };

    ASSERT_TRUE(test(Range(0, 0), "1", {}));
    ASSERT_TRUE(test(Range(0, 1), "1", { { Range(0, 1), "1" }}));
    ASSERT_TRUE(test(Range(0, 2), "2", { { Range(0, 2), "2" } }));
    ASSERT_TRUE(test(Range(5, 4), "3", { { Range(5, 4), "3" } }));
}


TEST(RangeMapTest, AddRangeWithDifferentValue) {

    auto test = [](
        const Range& range,
        const std::string& value,
        const RangeMap<std::string>::ItemList& expected) {

        RangeMap<std::string> map;
        map.AddRange(Range{ 0, 2 }, "0");
        map.AddRange(Range{ 5, 1 }, "1");
        map.AddRange(Range{ 10, 3 }, "1");
        map.AddRange(Range{ 15, 6 }, "3");
        map.AddRange(range, value);
        return CheckRangeMapResult(map, expected);
    };

    //Add a range that contains all existing ranges.
    ASSERT_TRUE(test(Range(0, 22), "A", { { Range(0, 22), "A" }}));

    //Add a new range that is equal to one of the existing ranges.
    ASSERT_TRUE(test(Range(5, 1), "A", {
        { Range(0, 2), "0" },
        { Range(5, 1), "A" },
        { Range(10, 3), "1" },
        { Range(15, 6), "3" },
    }));

    //Add a new range that doesn't intersect with any range.
    ASSERT_TRUE(test(Range(2, 3), "A", {
        { Range(0, 2), "0" }, 
        { Range(2, 3), "A" },
        { Range(5, 1), "1" },
        { Range(10, 3), "1" },
        { Range(15, 6), "3" },
    }));

    //Add a range that is a sub range of an existing range.
    ASSERT_TRUE(test(Range(17, 2), "A", {
        { Range(0, 2), "0" },
        { Range(5, 1), "1" },
        { Range(10, 3), "1" },
        { Range(15, 2), "3" },
        { Range(17, 2), "A" },
        { Range(19, 2), "3" },
    }));

    //Add a range that intersects with the head and the tail of existing ranges.
    ASSERT_TRUE(test(Range::FromIndexPair(12, 18), "A", {
        { Range(0, 2), "0" },
        { Range(5, 1), "1" },
        { Range(10, 2), "1" },
        { Range(12, 6), "A" },
        { Range(18, 3), "3" },
    }));
}


TEST(RangeMapTest, AddRangeWithSameValue) {

    auto test = [](
        const Range& range,
        const std::string& value,
        const RangeMap<std::string>::ItemList& expected) {

        RangeMap<std::string> map;
        map.AddRange(Range{ 0, 2 }, "0");
        map.AddRange(Range{ 5, 1 }, "1");
        map.AddRange(Range{ 10, 3 }, "1");
        map.AddRange(Range{ 15, 6 }, "3");
        map.AddRange(range, value);
        return CheckRangeMapResult(map, expected);
    };

    //Add a range that contains all existing ranges.
    ASSERT_TRUE(test(Range(0, 22), "1", { { Range(0, 22), "1" } }));

    //Add a new range that is equal to one of the existing ranges.
    ASSERT_TRUE(test(Range(5, 1), "1", {
        { Range(0, 2), "0" },
        { Range(5, 1), "1" },
        { Range(10, 3), "1" },
        { Range(15, 6), "3" },
    }));

    //Add a new range that doesn't intersect with any range.
    ASSERT_TRUE(test(Range::FromIndexPair(7, 9), "1", {
        { Range(0, 2), "0" },
        { Range(5, 1), "1" },
        { Range(7, 2), "1" },
        { Range(10, 3), "1" },
        { Range(15, 6), "3" },
    }));

    //Add a new range that is right before the existing range.
    ASSERT_TRUE(test(Range::FromIndexPair(8, 10), "1", {
        { Range(0, 2), "0" },
        { Range(5, 1), "1" },
        { Range(8, 5), "1" },
        { Range(15, 6), "3" },
    }));

    //Add a new range that is right before the existing range, and intersects with previous range.
    ASSERT_TRUE(test(Range::FromIndexPair(5, 10), "1", {
        { Range(0, 2), "0" },
        { Range(5, 8), "1" },
        { Range(15, 6), "3" },
    }));

    //Add a new range that connects two existing range.
    ASSERT_TRUE(test(Range::FromIndexPair(6, 10), "1", {
        { Range(0, 2), "0" },
        { Range(5, 8), "1" },
        { Range(15, 6), "3" },
    }));

    //Add a new range that is right after the existing range.
    ASSERT_TRUE(test(Range::FromIndexPair(6, 8), "1", {
        { Range(0, 2), "0" },
        { Range(5, 3), "1" },
        { Range(10, 3), "1" },
        { Range(15, 6), "3" },
    }));

    //Add a new range that is right after the existing range, and intersects with next range.
    ASSERT_TRUE(test(Range::FromIndexPair(6, 12), "1", {
        { Range(0, 2), "0" },
        { Range(5, 8), "1" },
        { Range(15, 6), "3" },
    }));

    //Add a range that is a sub range of an existing range.
    ASSERT_TRUE(test(Range(11, 1), "1", {
        { Range(0, 2), "0" },
        { Range(5, 1), "1" },
        { Range(10, 3), "1" },
        { Range(15, 6), "3" },
    }));

    //Add a range that intersects with the head and the tail of existing ranges.
    ASSERT_TRUE(test(Range::FromIndexPair(5, 12), "1", {
        { Range(0, 2), "0" },
        { Range(5, 8), "1" },
        { Range(15, 6), "3" },
    }));
}


TEST(RangeMapTest, RemoveRange) {

    auto test = [](const Range& range, const RangeMap<std::string>::ItemList& expected) {
        RangeMap<std::string> map;
        map.AddRange(Range(0, 2), "0");
        map.AddRange(Range(5, 1), "1");
        map.AddRange(Range(10, 3), "2");
        map.AddRange(Range(15, 6), "3");
        map.RemoveRange(range);
        return CheckRangeMapResult(map, expected);
    };

    ASSERT_TRUE(test(Range(2, 2), { 
        { Range(0, 2), "0" },
        { Range(5, 1), "1" },
        { Range(10, 3), "2" },
        { Range(15, 6), "3" }
    }));

    ASSERT_TRUE(test(Range(30, 5), {
        { Range(0, 2), "0" },
        { Range(5, 1), "1" },
        { Range(10, 3), "2" }, 
        { Range(15, 6), "3" },
    }));

    ASSERT_TRUE(test(Range(0, 3), {
        { Range(5, 1), "1" },
        { Range(10, 3), "2" },
        { Range(15, 6), "3" },
    }));

    ASSERT_TRUE(test(Range(4, 3), {
        { Range(0, 2), "0", },
        { Range(10, 3), "2" },
        { Range(15, 6), "3" },
    }));

    ASSERT_TRUE(test(Range(0, 30), { }));

    ASSERT_TRUE(test(Range(11, 1), {
        { Range(0, 2), "0" },
        { Range(5, 1), "1" },
        { Range(10, 1), "2" },
        { Range(12, 1), "2" },
        { Range(15, 6), "3" },
    }));

    ASSERT_TRUE(test(Range(17, 3), {
        { Range(0, 2), "0" },
        { Range(5, 1), "1" },
        { Range(10, 3), "2"},
        { Range(15, 2), "3" },
        { Range(20, 1), "3" },
    }));

    ASSERT_TRUE(test(Range(0, 1), {
        { Range(1, 1), "0" },
        { Range(5, 1), "1" },
        { Range(10, 3), "2" },
        { Range(15, 6), "3" },
    }));

    ASSERT_TRUE(test(Range(1, 1), {
        { Range(0, 1), "0" },
        { Range(5, 1), "1" },
        { Range(10, 3), "2" },
        { Range(15, 6), "3" },
    }));

    ASSERT_TRUE(test(Range(13, 3), {
        { Range(0, 2), "0" },
        { Range(5, 1), "1" },
        { Range(10, 3), "2" },
        { Range(16, 5), "3" }, 
    }));

    ASSERT_TRUE(test(Range(19, 5), { 
        { Range(0, 2), "0" }, 
        { Range(5, 1), "1" },
        { Range(10, 3), "2" },
        { Range(15, 4), "3" },
    }));
}


TEST(RangeMapTest, InsertSpan) {

    auto test = [](const Range& span, const RangeMap<std::string>::ItemList& expected) {

        RangeMap<std::string> map;
        map.AddRange(Range{ 5, 4 }, "0");
        map.AddRange(Range{ 20, 4 }, "1");

        map.InsertSpan(span);
        return CheckRangeMapResult(map, expected);
    };

    ASSERT_TRUE(test(Range(0, 0), { { Range(5, 4), "0" }, { Range(20, 4), "1" } }));
    ASSERT_TRUE(test(Range(6, 0), { { Range(5, 4), "0" }, { Range(20, 4), "1" } }));
    ASSERT_TRUE(test(Range(10, 0), { { Range(5, 4), "0" }, { Range(20, 4), "1" } }));
    ASSERT_TRUE(test(Range(22, 0), { { Range(5, 4), "0" }, { Range(20, 4), "1" } }));

    ASSERT_TRUE(test(Range(1, 2), { { Range(7, 4), "0" }, { Range(22, 4), "1" } }));
    ASSERT_TRUE(test(Range(10, 3), { { Range(5, 4), "0" }, { Range(23, 4), "1" } }));

    ASSERT_TRUE(test(Range(6, 2), { 
        { Range(5, 1), "0" }, 
        { Range(8, 3), "0" },
        { Range(22, 4), "1"}
    }));

    ASSERT_TRUE(test(Range(22, 2), { 
        { Range(5, 4), "0" },
        { Range(20, 2), "1" }, 
        { Range(24, 2), "1" }
    }));

    ASSERT_TRUE(test(Range(25, 3), { { Range(5, 4), "0" }, { Range(20, 4), "1" } }));
}


TEST(RangeMapTest, EraseSpan) {

    auto test = [](const Range& span, const RangeMap<std::string>::ItemList& expected) {

        RangeMap<std::string> map;
        map.AddRange(Range{ 5, 4 }, "0");
        map.AddRange(Range{ 20, 4 }, "1");

        map.EraseSpan(span);
        return CheckRangeMapResult(map, expected);
    };

    ASSERT_TRUE(test(Range(0, 0), { { Range(5, 4), "0" }, { Range(20, 4), "1" } }));
    ASSERT_TRUE(test(Range(6, 0), { { Range(5, 4), "0" }, { Range(20, 4), "1" } }));
    ASSERT_TRUE(test(Range(18, 0), { { Range(5, 4), "0" }, { Range(20, 4), "1" } }));
    ASSERT_TRUE(test(Range(22, 0), { { Range(5, 4), "0" }, { Range(20, 4), "1" } }));

    ASSERT_TRUE(test(Range(0, 2), { { Range(3, 4), "0" }, { Range(18, 4), "1" } }));
    ASSERT_TRUE(test(Range(10, 3), { { Range(5, 4), "0" }, { Range(17, 4), "1" } }));

    ASSERT_TRUE(test(Range(3, 10), { { Range(10, 4), "1" } }));
    ASSERT_TRUE(test(Range(3, 30), { }));

    ASSERT_TRUE(test(Range(6, 2), { { Range(5, 2), "0" }, { Range(18, 4), "1" } }));
    ASSERT_TRUE(test(Range(21, 2), { { Range(5, 4), "0" }, { Range(20, 2), "1" } }));

    ASSERT_TRUE(test(Range(3, 3), { { Range(3, 3), "0" }, { Range(17, 4), "1" } }));
    ASSERT_TRUE(test(Range(17, 5), { { Range(5, 4), "0" }, { Range(17, 2), "1" } }));

    ASSERT_TRUE(test(Range(7, 3), { { Range(5, 2), "0" }, { Range(17, 4), "1" } }));
    ASSERT_TRUE(test(Range(23, 5), { { Range(5, 4), "0" }, { Range(20, 3), "1" } }));

    ASSERT_TRUE(test(Range(30, 3), { { Range(5, 4), "0" }, { Range(20, 4), "1" } }));
}