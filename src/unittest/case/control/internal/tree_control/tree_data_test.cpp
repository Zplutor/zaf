#include <gtest/gtest.h>
#include <zaf/control/internal/tree_control/tree_data.h>

using namespace zaf::internal;

TEST(TreeDataTest, Test) {

    TreeData tree_data;
    auto& map = tree_data.node_child_count_map_;

    map[std::vector<std::size_t>{}] = 7;
    map[std::vector<std::size_t>{ 1 }] = 5;
    map[std::vector<std::size_t>{ 1, 2 }] = 1;
    map[std::vector<std::size_t>{ 1, 2, 0 }] = 4;
    map[std::vector<std::size_t>{ 1, 2, 0, 1 }] = 3;
    map[std::vector<std::size_t>{ 1, 3 }] = 2;
    map[std::vector<std::size_t>{ 4 }] = 3;

    std::vector<std::pair<std::size_t, zaf::IndexPath>> expected_results{
        { 0, { 0 } },
        { 1, { 1 } },
        { 2, { 1, 0 } },
        { 3, { 1, 1 } },
        { 4, { 1, 2 } },
        { 5, { 1, 2, 0 } },
        { 6, { 1, 2, 0, 0 } },
        { 7, { 1, 2, 0, 1 } },
        { 8, { 1, 2, 0, 1, 0 } },
        { 9, { 1, 2, 0, 1, 1 } },
        { 10, { 1, 2, 0, 1, 2 } },
        { 11, { 1, 2, 0, 2 } },
        { 12, { 1, 2, 0, 3 } },
        { 13, { 1, 3 } },
        { 14, { 1, 3, 0 } },
        { 15, { 1, 3, 1 } },
        { 16, { 1, 4 } },
        { 17, { 2 } },
        { 18, { 3 } },
        { 19, { 4 } },
        { 20, { 4, 0 } },
        { 21, { 4, 1 } },
        { 22, { 4, 2 } },
        { 23, { 5 } },
        { 24, { 6 } },
        { 25, { } },
    };

    for (const auto& each_result : expected_results) {

        auto path = tree_data.GetIndexPathAtIndex(each_result.first);
        ASSERT_EQ(path, each_result.second);
    }
}