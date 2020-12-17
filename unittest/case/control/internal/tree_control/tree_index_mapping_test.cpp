#include <gtest/gtest.h>
#include <zaf/base/container/utility/range.h>
#include <zaf/control/internal/tree_control/tree_index_mapping.h>

using namespace zaf::internal;

class NodeInfo {
public:
    std::size_t global_index{};
    zaf::IndexPath index_path{};
};

class TreeIndexMappingTest : public testing::Test {
protected:
    void SetUp() override {

        mapping_.node_child_count_pairs.assign({
            { {},             7 },
            { { 1 },          5 },
            { { 1, 2 },       1 },
            { { 1, 2, 0 },    4 },
            { { 1, 2, 0, 1 }, 3 },
            { { 1, 3 },       2 },
            { { 4 },          3 },
        });

        all_nodes_.assign({
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
        });
    }

    TreeIndexMapping& GetMapping() {
        return mapping_;
    }

    const std::vector<NodeInfo>& GetAllNodes() const {
        return all_nodes_;
    }

private:
    TreeIndexMapping mapping_;
    std::vector<NodeInfo> all_nodes_;
};


TEST_F(TreeIndexMappingTest, GetIndexPathAtIndex) {

    auto& mapping = GetMapping();
    const auto& all_nodes = GetAllNodes();
    for (const auto& each_node : all_nodes) {

        auto path = mapping.GetIndexPathAtIndex(each_node.global_index);
        ASSERT_EQ(path, each_node.index_path);
    }

    //Get index path for invalid index
    auto path = mapping.GetIndexPathAtIndex(all_nodes.back().global_index + 1);
    ASSERT_EQ(path, zaf::IndexPath{});
}


TEST_F(TreeIndexMappingTest, GetIndexPathAtIndexOnSingleLevelTree) {

    const std::size_t node_count = 3;

    TreeIndexMapping mapping;
    mapping.node_child_count_pairs.push_back(std::make_pair(zaf::IndexPath{}, node_count));

    for (auto index : zaf::Range(node_count)) {

        auto index_path = mapping.GetIndexPathAtIndex(index);
        ASSERT_EQ(index_path, zaf::IndexPath{ index });
    }

    auto index_path = mapping.GetIndexPathAtIndex(node_count);
    ASSERT_EQ(index_path, zaf::IndexPath{});
}


TEST_F(TreeIndexMappingTest, GetIndexAtIndexPath) {

    auto& mapping = GetMapping();
    for (const auto& each_node : GetAllNodes()) {

        auto index = mapping.GetIndexAtIndexPath(each_node.index_path);
        ASSERT_TRUE(index.has_value());
        ASSERT_EQ(*index, each_node.global_index);
    }

    //Get index for invalid index path
    ASSERT_FALSE(mapping.GetIndexAtIndexPath({}).has_value());
    ASSERT_FALSE(mapping.GetIndexAtIndexPath({ 1, 2, 0, 0, 0 }).has_value());
    ASSERT_FALSE(mapping.GetIndexAtIndexPath({ 7, 0 }).has_value());
}


TEST_F(TreeIndexMappingTest, GetIndexAtIndexPathOnSingleLevelTree) {

    const std::size_t node_count = 3;

    TreeIndexMapping mapping;
    mapping.node_child_count_pairs.push_back(std::make_pair(zaf::IndexPath{}, node_count));

    for (auto index : zaf::Range(node_count)) {

        auto got_index = mapping.GetIndexAtIndexPath(zaf::IndexPath{ index });
        ASSERT_TRUE(got_index.has_value());
        ASSERT_EQ(*got_index, index);
    }
}


TEST_F(TreeIndexMappingTest, AddChildren) {

    TreeIndexMapping mapping;
    mapping.AddChildren({}, 0, 2);
    mapping.AddChildren({}, 0, 1);
    mapping.AddChildren({}, 3, 1);
    mapping.AddChildren({ 1 }, 0, 3);
    mapping.AddChildren({ 1, 0 }, 0, 1);
    mapping.AddChildren({}, 1, 1);

    std::vector<std::pair<zaf::IndexPath, std::size_t>> expected{
        { {}, 5 },
        { { 2 }, 3 },
        { { 2, 0 }, 1 },
    };
    ASSERT_EQ(expected, mapping.node_child_count_pairs);
}


TEST_F(TreeIndexMappingTest, RemoveChildren) {

    TreeIndexMapping mapping;
    mapping.node_child_count_pairs.assign({
        { {}, 3 },
        { { 1 }, 3 },
        { { 1, 1 }, 2 },
    });

    mapping.RemoveChildren({ 1, 1 }, 0, 1);
    std::vector<std::pair<zaf::IndexPath, std::size_t>> expected{
        { {}, 3 },
        { { 1 }, 3 },
        { { 1, 1 }, 1 },
    };
    ASSERT_EQ(expected, mapping.node_child_count_pairs);

    mapping.RemoveChildren({ 1, 1 }, 0, 1);
    expected.assign({
        { {}, 3 },
        { { 1 }, 3 },
    });
    ASSERT_EQ(expected, mapping.node_child_count_pairs);

    mapping.RemoveChildren({}, 0, 1);
    expected.assign({
        { {}, 2 },
        { { 0 }, 3 },
    });
    ASSERT_EQ(expected, mapping.node_child_count_pairs);
}


TEST_F(TreeIndexMappingTest, RemoveChildrenRecursively) {

    TreeIndexMapping mapping;
    mapping.node_child_count_pairs.assign({
        { {}, 3 },
        { { 1 }, 3 },
        { { 1, 1 }, 2 },
    });

    auto remove_count = mapping.RemoveChildren({ 1 }, 0, 3);
    std::vector<std::pair<zaf::IndexPath, std::size_t>> expected{
        { {}, 3 },
    };
    ASSERT_EQ(expected, mapping.node_child_count_pairs);
    ASSERT_EQ(remove_count, 5);
}