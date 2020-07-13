#include <numeric>
#include <gtest/gtest.h>
#include <zaf/base/define.h>
#include <zaf/control/internal/list_control_item_height_manager.h>

class FakeItemSource : public zaf::ListItemSource {
public:
    std::size_t GetItemCount() override {
        return item_count;
    }

    bool HasVariableItemHeight() override {
        return has_variable_item_heights;
    }

    float GetItemHeight(std::size_t index) override {
        if (has_variable_item_heights) {
            return index % 10 + item_height + variable_item_height_adjustment;
        }
        else {
            return item_height;
        }
    }

    std::shared_ptr<zaf::ListItem> CreateItem(std::size_t index) override {
        return nullptr;
    }

    void AddItems(std::size_t index, std::size_t count) {
        item_count += count;
        NotifyItemAdd(index, count);
    }

    void RemoveItems(std::size_t index, std::size_t count) {
        item_count -= count;
        NotifyItemRemove(index, count);
    }

    void UpdateItems(std::size_t index, std::size_t count) {
        NotifyItemUpdate(index, count);
    }

public:
    std::size_t item_count = 0;
    bool has_variable_item_heights = false;
    float item_height = 10;
    float variable_item_height_adjustment = 0;
};


class ListControlItemHeightManagerTest : public testing::Test {
public:
    void SetUp() override {
        
        item_source_ = std::make_shared<FakeItemSource>();
        item_height_manager_ = std::make_shared<zaf::internal::ListControlItemHeightManager>(item_source_);
    }

protected:
    void CheckItemHeightManagerWithFixedItemHeights() const {

        ASSERT_EQ(item_source_->item_count, item_height_manager_->GetItemCount());
        ASSERT_EQ(item_source_->item_count * item_source_->item_height, item_height_manager_->GetTotalHeight());

        for (std::size_t index = 0; index < item_source_->item_count; ++index) {

            auto position_and_height = item_height_manager_->GetItemPositionAndHeight(index);
            ASSERT_EQ(index * item_source_->item_height, position_and_height.first);
            ASSERT_EQ(item_source_->item_height, position_and_height.second);
        }

        auto index_and_count = item_height_manager_->GetItemIndexAndCount(0, item_source_->item_count * item_source_->item_height);
        ASSERT_EQ(0, index_and_count.first);
        ASSERT_EQ(item_source_->item_count, index_and_count.second);
    }


    void CheckItemHeightManagerWithVariableItemHeights(const std::vector<float>& expected_item_heights) const {

        ASSERT_EQ(item_source_->item_count, item_height_manager_->GetItemCount());

        float total_height = 0;

        for (std::size_t index = 0; index < item_source_->item_count; ++index) {
            auto position_and_height = item_height_manager_->GetItemPositionAndHeight(index);
            ASSERT_EQ(total_height, position_and_height.first);
            ASSERT_EQ(expected_item_heights[index], position_and_height.second);
            total_height += expected_item_heights[index];
        }

        ASSERT_EQ(total_height, item_height_manager_->GetTotalHeight());

        auto index_and_count = item_height_manager_->GetItemIndexAndCount(0, 100);
        ASSERT_EQ(0, index_and_count.first);
        ASSERT_EQ(item_source_->item_count, index_and_count.second);
    }

    bool TestGetItemIndexAndCount(
        float begin_position,
        float end_position,
        std::size_t expected_index,
        std::size_t expected_count) const {

        auto index_and_count = item_height_manager_->GetItemIndexAndCount(begin_position, end_position);
        return (index_and_count.first == expected_index) && (index_and_count.second == expected_count);
    }

protected:
    std::shared_ptr<FakeItemSource> item_source_;
    std::shared_ptr<zaf::internal::ListControlItemHeightManager> item_height_manager_;
};


TEST_F(ListControlItemHeightManagerTest, GetItemCount) {

    auto test = [this]() {

        //No item
        item_source_->item_count = 0;
        item_height_manager_->LoadItemHeights();
        ASSERT_EQ(0, item_height_manager_->GetItemCount());

        //One item
        item_source_->item_count = 1;
        item_height_manager_->LoadItemHeights();
        ASSERT_EQ(1, item_height_manager_->GetItemCount());

        //Many items
        item_source_->item_count = 999;
        item_height_manager_->LoadItemHeights();
        ASSERT_EQ(999, item_height_manager_->GetItemCount());
    };
    
    //Fixed item heights
    item_source_->has_variable_item_heights = false;
    test();

    //Variable item heights
    item_source_->has_variable_item_heights = true;
    test();
}


TEST_F(ListControlItemHeightManagerTest, GetTotalHeight) {

    auto test = [this](std::size_t item_count, std::size_t expected_total_height) {
        item_source_->item_count = item_count;
        item_height_manager_->LoadItemHeights();
        ASSERT_EQ(expected_total_height, item_height_manager_->GetTotalHeight());
    };

    //Fixed item heights
    item_source_->has_variable_item_heights = false;
    test(0, 0);
    test(1, 10);
    test(999, 9990);

    //Variable item heights
    item_source_->has_variable_item_heights = true;
    test(0, 0);
    test(1, 10);
    test(2, 21);
    test(11, 155);
}


TEST_F(ListControlItemHeightManagerTest, GetItemPositionAndHeight) {

    //Fixed item heights
    item_source_->has_variable_item_heights = false;

    item_source_->item_count = 0;
    item_height_manager_->LoadItemHeights();
    auto position_and_height = item_height_manager_->GetItemPositionAndHeight(0);
    ASSERT_EQ(0, position_and_height.first);
    ASSERT_EQ(0, position_and_height.second);

    item_source_->item_count = 1;
    item_height_manager_->LoadItemHeights();
    position_and_height = item_height_manager_->GetItemPositionAndHeight(0);
    ASSERT_EQ(0, position_and_height.first);
    ASSERT_EQ(10, position_and_height.second);

    item_source_->item_count = 33;
    item_height_manager_->LoadItemHeights();
    for (std::size_t index = 0; index < item_source_->item_count; ++index) {
        position_and_height = item_height_manager_->GetItemPositionAndHeight(index);
        ASSERT_EQ(index * 10, position_and_height.first);
        ASSERT_EQ(10, position_and_height.second);
    }

    //Variable item heights
    item_source_->has_variable_item_heights = true;

    item_source_->item_count = 0;
    item_height_manager_->LoadItemHeights();
    position_and_height = item_height_manager_->GetItemPositionAndHeight(0);
    ASSERT_EQ(0, position_and_height.first);
    ASSERT_EQ(0, position_and_height.second);

    item_source_->item_count = 1;
    item_height_manager_->LoadItemHeights();
    position_and_height = item_height_manager_->GetItemPositionAndHeight(0);
    ASSERT_EQ(0, position_and_height.first);
    ASSERT_EQ(10, position_and_height.second);

    item_source_->item_count = 14;
    item_height_manager_->LoadItemHeights();
    position_and_height = item_height_manager_->GetItemPositionAndHeight(1);
    ASSERT_EQ(10, position_and_height.first);
    ASSERT_EQ(11, position_and_height.second);

    position_and_height = item_height_manager_->GetItemPositionAndHeight(2);
    ASSERT_EQ(21, position_and_height.first);
    ASSERT_EQ(12, position_and_height.second);

    position_and_height = item_height_manager_->GetItemPositionAndHeight(10);
    ASSERT_EQ(145, position_and_height.first);
    ASSERT_EQ(10, position_and_height.second);
}


TEST_F(ListControlItemHeightManagerTest, GetItemIndexAndCountWithFixedItemHeights) {

    item_source_->has_variable_item_heights = false;

    //No item
    item_source_->item_count = 0;
    item_height_manager_->LoadItemHeights();
    ASSERT_TRUE(TestGetItemIndexAndCount(0, 10, 0, 0));

    item_source_->item_count = 3;
    item_height_manager_->LoadItemHeights();

    //Invalid range
    ASSERT_TRUE(TestGetItemIndexAndCount(-1, -3, 0, 0));
    ASSERT_TRUE(TestGetItemIndexAndCount(4, 0, 0, 0));

    //Valid range
    ASSERT_TRUE(TestGetItemIndexAndCount(0, 0, 0, 1));
    ASSERT_TRUE(TestGetItemIndexAndCount(5, 5, 0, 1));
    ASSERT_TRUE(TestGetItemIndexAndCount(10, 10, 1, 1));
    ASSERT_TRUE(TestGetItemIndexAndCount(20, 20, 2, 1));

    ASSERT_TRUE(TestGetItemIndexAndCount(0, 1, 0, 1));
    ASSERT_TRUE(TestGetItemIndexAndCount(0, 10, 0, 1));
    ASSERT_TRUE(TestGetItemIndexAndCount(0, 30, 0, 3));
    ASSERT_TRUE(TestGetItemIndexAndCount(0, 100, 0, 3));
    ASSERT_TRUE(TestGetItemIndexAndCount(11, 24, 1, 2));

    //Out of range
    ASSERT_TRUE(TestGetItemIndexAndCount(30, 30, 0, 0));
    ASSERT_TRUE(TestGetItemIndexAndCount(30, 40, 0, 0));
    ASSERT_TRUE(TestGetItemIndexAndCount(31, 32, 0, 0));
}


TEST_F(ListControlItemHeightManagerTest, GetItemIndexAndCountWithVariableItemHeights) {

    item_source_->has_variable_item_heights = true;

    //No item
    item_source_->item_count = 0;
    item_height_manager_->LoadItemHeights();
    ASSERT_TRUE(TestGetItemIndexAndCount(0, 10, 0, 0));

    item_source_->item_count = 3;
    item_height_manager_->LoadItemHeights();

    //Invalid range
    ASSERT_TRUE(TestGetItemIndexAndCount(1, 0, 0, 0));
    ASSERT_TRUE(TestGetItemIndexAndCount(4, 0, 0, 0));
    ASSERT_TRUE(TestGetItemIndexAndCount(-1, -1, 0, 0));
    ASSERT_TRUE(TestGetItemIndexAndCount(-1, 1, 0, 0));

    //Valid range
    ASSERT_TRUE(TestGetItemIndexAndCount(0, 0, 0, 1));
    ASSERT_TRUE(TestGetItemIndexAndCount(5, 5, 0, 1));
    ASSERT_TRUE(TestGetItemIndexAndCount(10, 10, 1, 1));
    ASSERT_TRUE(TestGetItemIndexAndCount(21, 21, 2, 1));

    ASSERT_TRUE(TestGetItemIndexAndCount(0, 10, 0, 1));
    ASSERT_TRUE(TestGetItemIndexAndCount(0, 30, 0, 3));
    ASSERT_TRUE(TestGetItemIndexAndCount(0, 100, 0, 3));
    ASSERT_TRUE(TestGetItemIndexAndCount(10, 20, 1, 1));
    ASSERT_TRUE(TestGetItemIndexAndCount(11, 21, 1, 1));
    ASSERT_TRUE(TestGetItemIndexAndCount(11, 24, 1, 2));
    ASSERT_TRUE(TestGetItemIndexAndCount(20, 30, 1, 2));

    //Ouf of range
    ASSERT_TRUE(TestGetItemIndexAndCount(33, 40, 0, 0));
    ASSERT_TRUE(TestGetItemIndexAndCount(34, 50, 0, 0));
}


TEST_F(ListControlItemHeightManagerTest, AddItemsWithFixedItemHeights) {

    item_source_->has_variable_item_heights = false;

    auto test_add_items = [this](std::size_t initial_count, std::size_t add_index, std::size_t add_count) {

        item_source_->item_count = initial_count;
        item_height_manager_->LoadItemHeights();
        item_source_->AddItems(add_index, add_count);
        CheckItemHeightManagerWithFixedItemHeights();
    };

    //Add items to empty item source.
    test_add_items(0, 0, 1);
    test_add_items(0, 0, 3);

    //Add items to non-empty item source.
    //Add to head
    test_add_items(1, 0, 1);
    test_add_items(1, 0, 2);
    test_add_items(10, 0, 1);
    test_add_items(10, 0, 10);

    //Add to middle
    test_add_items(10, 4, 1);
    test_add_items(10, 4, 20);

    //Add to tail
    test_add_items(1, 1, 1);
    test_add_items(1, 1, 2);
    test_add_items(10, 10, 1);
    test_add_items(10, 10, 4);
}


TEST_F(ListControlItemHeightManagerTest, AddItemsWithVariableItemHeights) {

    item_source_->has_variable_item_heights = true;

    auto test_add_items = [this](
        std::size_t initial_count, 
        std::size_t add_index,
        std::size_t add_count,
        const std::vector<float>& expected_item_heights) {

        item_source_->item_count = initial_count;
        item_height_manager_->LoadItemHeights();
        item_source_->AddItems(add_index, add_count);

        CheckItemHeightManagerWithVariableItemHeights(expected_item_heights);
    };

    //Add items to empty item source
    test_add_items(0, 0, 1, { 10 });
    test_add_items(0, 0, 2, { 10, 11 });

    //Add items to non-empty item source
    //Add to head
    test_add_items(1, 0, 1, { 10, 10 });
    test_add_items(1, 0, 2, { 10, 11, 10 });
    test_add_items(5, 0, 1, { 10, 10, 11, 12, 13, 14 });
    test_add_items(5, 0, 2, { 10, 11, 10, 11, 12, 13, 14 });

    //Add to middle
    test_add_items(5, 3, 1, { 10, 11, 12, 13, 13, 14 });
    test_add_items(5, 3, 2, { 10, 11, 12, 13, 14, 13, 14 });

    //Add to tail
    test_add_items(1, 1, 1, { 10, 11 });
    test_add_items(1, 1, 2, { 10, 11, 12 });
    test_add_items(5, 5, 1, { 10, 11, 12, 13, 14, 15 });
    test_add_items(5, 5, 2, { 10, 11, 12, 13, 14, 15, 16 });
}


TEST_F(ListControlItemHeightManagerTest, RemoveItemsWithFixedItemHeights) {

    item_source_->has_variable_item_heights = false;

    auto test_remove_items = [this](std::size_t initial_count, std::size_t remove_index, std::size_t remove_count) {

        item_source_->item_count = initial_count;
        item_height_manager_->LoadItemHeights();

        item_source_->RemoveItems(remove_index, remove_count);
        CheckItemHeightManagerWithFixedItemHeights();
    };

    //Remove all items from item source.
    test_remove_items(1, 0, 1);
    test_remove_items(2, 0, 2);
    test_remove_items(10, 0, 10);

    //Remove partial items.
    //Remove from head
    test_remove_items(2, 0, 1);
    test_remove_items(4, 0, 2);

    //Remove from middle
    test_remove_items(3, 1, 1);
    test_remove_items(6, 2, 2);

    //Remove from tail
    test_remove_items(2, 1, 1);
    test_remove_items(7, 5, 2);
}


TEST_F(ListControlItemHeightManagerTest, RemoveItemsWithVariableItemHeights) {

    item_source_->has_variable_item_heights = true;

    auto test_remove_items = [this](
        std::size_t initial_count, 
        std::size_t remove_index, 
        std::size_t remove_count,
        const std::vector<float>& expected_item_heights) {

        item_source_->item_count = initial_count;
        item_height_manager_->LoadItemHeights();

        item_source_->RemoveItems(remove_index, remove_count);
        CheckItemHeightManagerWithVariableItemHeights(expected_item_heights);
    };

    //Remove all items
    test_remove_items(1, 0, 1, { });
    test_remove_items(3, 0, 3, { });

    //Remove partial items
    //Remove from head
    test_remove_items(2, 0, 1, { 11 });
    test_remove_items(5, 0, 3, { 13, 14 });

    //Remove from middle
    test_remove_items(3, 1, 1, { 10, 12 });
    test_remove_items(4, 1, 2, { 10, 13 });

    //Remove from tail
    test_remove_items(2, 1, 1, { 10 });
    test_remove_items(5, 3, 2, { 10, 11, 12 });
}


TEST_F(ListControlItemHeightManagerTest, UpdateItemsWithFixedItemHeights) {

    item_source_->has_variable_item_heights = false;

    auto test_update_items = [this](std::size_t initial_item_count, std::size_t update_index, std::size_t update_count) {

        item_source_->item_count = initial_item_count;
        item_height_manager_->LoadItemHeights();

        item_source_->UpdateItems(update_index, update_count);
        CheckItemHeightManagerWithFixedItemHeights();
    };

    //Update all items
    test_update_items(1, 0, 1);
    test_update_items(8, 0, 8);

    //Update partial items
    //Update head 
    test_update_items(2, 0, 1);
    test_update_items(7, 0, 2);

    //Update middle
    test_update_items(3, 1, 1);
    test_update_items(9, 4, 3);

    //Update tail
    test_update_items(2, 1, 1);
    test_update_items(9, 7, 2);
}


TEST_F(ListControlItemHeightManagerTest, UpdateItemsWithVariableItemHeights) {

    item_source_->has_variable_item_heights = true;

    auto test_update_items = [this](
        std::size_t initial_item_count, 
        std::size_t update_index, 
        std::size_t update_count,
        const std::vector<float>& expected_item_heights) {

        item_source_->variable_item_height_adjustment = 0;
        item_source_->item_count = initial_item_count;
        item_height_manager_->LoadItemHeights();

        item_source_->variable_item_height_adjustment = 1;
        item_source_->UpdateItems(update_index, update_count);
        CheckItemHeightManagerWithVariableItemHeights(expected_item_heights);
    };

    //Update all items.
    test_update_items(1, 0, 1, { 11 });
    test_update_items(4, 0, 4, { 11, 12, 13, 14 });

    //Update partial items.
    //Update head
    test_update_items(2, 0, 1, { 11, 11 });
    test_update_items(6, 0, 3, { 11, 12, 13, 13, 14, 15 });

    //Update middle
    test_update_items(3, 1, 1, { 10, 12, 12 });
    test_update_items(6, 2, 3, { 10, 11, 13, 14, 15, 15 });

    //Update tail
    test_update_items(2, 1, 1, { 10, 12 });
    test_update_items(7, 4, 3, { 10, 11, 12, 13, 15, 16, 17 });
}