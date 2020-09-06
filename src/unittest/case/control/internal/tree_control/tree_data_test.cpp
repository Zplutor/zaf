#include <gtest/gtest.h>
#include <zaf/control/internal/tree_control/tree_data.h>

using namespace zaf::internal;

class NodeInfo {
public:
    std::size_t global_index{};
    zaf::IndexPath index_path{};
};

class TreeDataTest : public testing::Test {
protected:
    void SetUp() override {

        auto& map = tree_data_.node_child_count_map_;
        map[std::vector<std::size_t>{}] = 7;
        map[std::vector<std::size_t>{ 1 }] = 5;
        map[std::vector<std::size_t>{ 1, 2 }] = 1;
        map[std::vector<std::size_t>{ 1, 2, 0 }] = 4;
        map[std::vector<std::size_t>{ 1, 2, 0, 1 }] = 3;
        map[std::vector<std::size_t>{ 1, 3 }] = 2;
        map[std::vector<std::size_t>{ 4 }] = 3;

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

    TreeData& GetTreeData() {
        return tree_data_;
    }

    const std::vector<NodeInfo>& GetAllNodes() const {
        return all_nodes_;
    }

private:
    TreeData tree_data_;
    std::vector<NodeInfo> all_nodes_;
};


TEST_F(TreeDataTest, GetIndexPathAtIndex) {

    auto& tree_data = GetTreeData();
    const auto& all_nodes = GetAllNodes();
    for (const auto& each_node : all_nodes) {

        auto path = tree_data.GetIndexPathAtIndex(each_node.global_index);
        ASSERT_EQ(path, each_node.index_path);
    }

    //Get index path for invalid index
    auto path = tree_data.GetIndexPathAtIndex(all_nodes.back().global_index + 1);
    ASSERT_EQ(path, zaf::IndexPath{});
}


TEST_F(TreeDataTest, GetIndexAtIndexPath) {

    auto& tree_data = GetTreeData();
    for (const auto& each_node : GetAllNodes()) {

        auto index = tree_data.GetIndexAtIndexPath(each_node.index_path);
        ASSERT_EQ(index, each_node.global_index);
    }

    //Get index for invalid index path
    ASSERT_EQ(zaf::InvalidIndex, tree_data.GetIndexAtIndexPath({}));
    ASSERT_EQ(zaf::InvalidIndex, tree_data.GetIndexAtIndexPath({ 7, 0 }));
}