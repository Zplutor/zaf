#include <numeric>
#include <gtest/gtest.h>
#include <zaf/base/range.h>
#include <zaf/base/define.h>
#include <zaf/internal/list/list_item_height_manager.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>

class FakeItemSource : public zaf::ListDataSource, public zaf::ListControlDelegate {
public:
    std::size_t GetDataCount() const override {
        return item_count;
    }

    bool HasVariableItemHeight() override {
        return has_variable_item_heights;
    }

    float EstimateItemHeight(
        std::size_t index, 
        const std::shared_ptr<zaf::Object>& item_data) override {

        if (has_variable_item_heights) {
            return index % 10 + item_height + variable_item_height_adjustment;
        }
        else {
            return item_height;
        }
    }

    float GetItemSpacing() override {
        return item_spacing;
    }

    void AddItems(std::size_t index, std::size_t count) {
        item_count += count;
        NotifyDataAdded(index, count);
    }

    void RemoveItems(std::size_t index, std::size_t count) {
        item_count -= count;
        NotifyDataRemoved(index, count);
    }

    void UpdateItems(std::size_t index, std::size_t count) {
        NotifyDataUpdated(index, count);
    }

public:
    std::size_t item_count = 0;
    bool has_variable_item_heights = false;
    float item_height = 10;
    float item_spacing = 0;
    float variable_item_height_adjustment = 0;
};


class ListControlItemHeightManagerTest : public testing::Test {
public:
    void SetUp() override {
        
        item_source_ = std::make_shared<FakeItemSource>();

        item_height_manager_ = std::make_shared<zaf::internal::ListItemHeightManager>();

        item_height_manager_->ResetDataSource(item_source_);
        item_height_manager_->ResetDelegate(item_source_);
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

        auto index_and_count = item_height_manager_->GetItemRange(0, item_source_->item_count * item_source_->item_height);
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

        auto index_and_count = item_height_manager_->GetItemRange(0, 100);
        ASSERT_EQ(0, index_and_count.first);
        ASSERT_EQ(item_source_->item_count, index_and_count.second);
    }

    bool TestGetItemRange(
        float begin_position,
        float end_position,
        std::size_t expected_index,
        std::size_t expected_count) const {

        auto index_and_count = item_height_manager_->GetItemRange(begin_position, end_position);
        return (index_and_count.first == expected_index) && (index_and_count.second == expected_count);
    }

    bool CheckItemPositionsAndHeights(const std::vector<std::pair<float, float>>& expected) {

        if (item_height_manager_->GetItemCount() != expected.size()) {
            return false;
        }

        for (auto index : zaf::Range(0, expected.size())) {
            if (item_height_manager_->GetItemPositionAndHeight(index) != expected[index]) {
                return false;
            }
        }

        const auto& last = expected.back();
        return item_height_manager_->GetTotalHeight() == last.first + last.second;
    }

protected:
    std::shared_ptr<FakeItemSource> item_source_;
    std::shared_ptr<zaf::internal::ListItemHeightManager> item_height_manager_;
};


TEST_F(ListControlItemHeightManagerTest, GetItemCount) {

    auto test = [this]() {

        //No item
        item_source_->item_count = 0;
        item_height_manager_->ReloadItemHeights();
        ASSERT_EQ(0, item_height_manager_->GetItemCount());

        //One item
        item_source_->item_count = 1;
        item_height_manager_->ReloadItemHeights();
        ASSERT_EQ(1, item_height_manager_->GetItemCount());

        //Many items
        item_source_->item_count = 999;
        item_height_manager_->ReloadItemHeights();
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
        item_height_manager_->ReloadItemHeights();
        ASSERT_EQ(expected_total_height, item_height_manager_->GetTotalHeight());
    };

    //Fixed item heights without spacing
    item_source_->has_variable_item_heights = false;
    item_source_->item_spacing = 0;
    test(0, 0);
    test(1, 10);
    test(999, 9990);

    //Fixed item heights with spacing
    item_source_->has_variable_item_heights = false;
    item_source_->item_spacing = 2;
    test(0, 0);
    test(1, 10);
    test(5, 58);
    
    //Variable item heights without spacing
    item_source_->has_variable_item_heights = true;
    item_source_->item_spacing = 0;
    test(0, 0);
    test(1, 10);
    test(2, 21);
    test(11, 155);

    //Variable item heights with spacing
    item_source_->has_variable_item_heights = true;
    item_source_->item_spacing = 2;
    test(0, 0);
    test(1, 10);
    test(2, 23);
    test(11, 175);
}


TEST_F(ListControlItemHeightManagerTest, GetItemPositionAndHeight_FixedHeight_NoSpacing) {

    item_source_->has_variable_item_heights = false;

    item_source_->item_count = 0;
    item_height_manager_->ReloadItemHeights();
    auto position_and_height = item_height_manager_->GetItemPositionAndHeight(0);
    ASSERT_EQ(0, position_and_height.first);
    ASSERT_EQ(0, position_and_height.second);

    item_source_->item_count = 1;
    item_height_manager_->ReloadItemHeights();
    position_and_height = item_height_manager_->GetItemPositionAndHeight(0);
    ASSERT_EQ(0, position_and_height.first);
    ASSERT_EQ(10, position_and_height.second);

    item_source_->item_count = 33;
    item_height_manager_->ReloadItemHeights();
    for (std::size_t index = 0; index < item_source_->item_count; ++index) {
        position_and_height = item_height_manager_->GetItemPositionAndHeight(index);
        ASSERT_EQ(index * 10, position_and_height.first);
        ASSERT_EQ(10, position_and_height.second);
    }
}


TEST_F(ListControlItemHeightManagerTest, GetItemPositionAndHeight_FixedHeight_Spacing) {

    item_source_->has_variable_item_heights = false;
    item_source_->item_spacing = 4;

    item_source_->item_count = 3;
    item_height_manager_->ReloadItemHeights();

    auto result = item_height_manager_->GetItemPositionAndHeight(0);
    ASSERT_EQ(result.first, 0);
    ASSERT_EQ(result.second, 10);

    result = item_height_manager_->GetItemPositionAndHeight(1);
    ASSERT_EQ(result.first, 14);
    ASSERT_EQ(result.second, 10);

    result = item_height_manager_->GetItemPositionAndHeight(2);
    ASSERT_EQ(result.first, 28);
    ASSERT_EQ(result.second, 10);

    result = item_height_manager_->GetItemPositionAndHeight(3);
    ASSERT_EQ(result.first, 0);
    ASSERT_EQ(result.second, 0);
}


TEST_F(ListControlItemHeightManagerTest, GetItemPositionAndHeight_VariableHeight_NoSpacing) {

    item_source_->has_variable_item_heights = true;

    item_source_->item_count = 0;
    item_height_manager_->ReloadItemHeights();
    auto position_and_height = item_height_manager_->GetItemPositionAndHeight(0);
    ASSERT_EQ(0, position_and_height.first);
    ASSERT_EQ(0, position_and_height.second);

    item_source_->item_count = 1;
    item_height_manager_->ReloadItemHeights();
    position_and_height = item_height_manager_->GetItemPositionAndHeight(0);
    ASSERT_EQ(0, position_and_height.first);
    ASSERT_EQ(10, position_and_height.second);

    item_source_->item_count = 14;
    item_height_manager_->ReloadItemHeights();
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


TEST_F(ListControlItemHeightManagerTest, GetItemPositionAndHeight_VariableHeight_Spacing) {

    item_source_->has_variable_item_heights = true;
    item_source_->item_spacing = 4;
    item_source_->item_count = 3;
    item_height_manager_->ReloadItemHeights();

    auto position_and_height = item_height_manager_->GetItemPositionAndHeight(0);
    ASSERT_EQ(position_and_height.first, 0);
    ASSERT_EQ(position_and_height.second, 10);

    position_and_height = item_height_manager_->GetItemPositionAndHeight(1);
    ASSERT_EQ(position_and_height.first, 14);
    ASSERT_EQ(position_and_height.second, 11);

    position_and_height = item_height_manager_->GetItemPositionAndHeight(2);
    ASSERT_EQ(position_and_height.first, 29);
    ASSERT_EQ(position_and_height.second, 12);
}


TEST_F(ListControlItemHeightManagerTest, GetItemRange_FixedHeight_NoSpacing) {

    item_source_->has_variable_item_heights = false;

    //No item
    item_source_->item_count = 0;
    item_height_manager_->ReloadItemHeights();
    ASSERT_TRUE(TestGetItemRange(0, 10, 0, 0));

    item_source_->item_count = 3;
    item_height_manager_->ReloadItemHeights();

    //Invalid range
    ASSERT_TRUE(TestGetItemRange(-1, -3, 0, 0));
    ASSERT_TRUE(TestGetItemRange(-10, 20, 0, 0));
    ASSERT_TRUE(TestGetItemRange(4, 0, 0, 0));

    //Empty range
    ASSERT_TRUE(TestGetItemRange(0, 0, 0, 0));
    ASSERT_TRUE(TestGetItemRange(5, 5, 0, 0));
    ASSERT_TRUE(TestGetItemRange(10, 10, 0, 0));
    ASSERT_TRUE(TestGetItemRange(20, 20, 0, 0));

    //Valid range
    ASSERT_TRUE(TestGetItemRange(0, 1, 0, 1));
    ASSERT_TRUE(TestGetItemRange(0, 10, 0, 1));
    ASSERT_TRUE(TestGetItemRange(0, 30, 0, 3));
    ASSERT_TRUE(TestGetItemRange(0, 100, 0, 3));
    ASSERT_TRUE(TestGetItemRange(11, 24, 1, 2));

    //Out of range
    ASSERT_TRUE(TestGetItemRange(30, 30, 0, 0));
    ASSERT_TRUE(TestGetItemRange(30, 40, 0, 0));
    ASSERT_TRUE(TestGetItemRange(31, 32, 0, 0));
}


TEST_F(ListControlItemHeightManagerTest, GetItemRange_FixedHeight_Spacing) {

    item_source_->has_variable_item_heights = false;
    item_source_->item_spacing = 6;
    item_source_->item_count = 3;
    item_height_manager_->ReloadItemHeights();

    ASSERT_TRUE(TestGetItemRange(0, 5, 0, 1));
    ASSERT_TRUE(TestGetItemRange(0, 13, 0, 1));
    ASSERT_TRUE(TestGetItemRange(0, 16, 0, 1));
    ASSERT_TRUE(TestGetItemRange(0, 20, 0, 2));
    ASSERT_TRUE(TestGetItemRange(0, 32, 0, 2));
    ASSERT_TRUE(TestGetItemRange(0, 100, 0, 3));

    ASSERT_TRUE(TestGetItemRange(13, 16, 1, 0));
    ASSERT_TRUE(TestGetItemRange(13, 24, 1, 1));

    ASSERT_TRUE(TestGetItemRange(42, 43, 0, 0));
}


TEST_F(ListControlItemHeightManagerTest, GetItemRange_VariableHeight_NoSpacing) {

    item_source_->has_variable_item_heights = true;

    //No item
    item_source_->item_count = 0;
    item_height_manager_->ReloadItemHeights();
    ASSERT_TRUE(TestGetItemRange(0, 10, 0, 0));

    item_source_->item_count = 3;
    item_height_manager_->ReloadItemHeights();

    //Invalid range
    ASSERT_TRUE(TestGetItemRange(1, 0, 0, 0));
    ASSERT_TRUE(TestGetItemRange(4, 0, 0, 0));
    ASSERT_TRUE(TestGetItemRange(-1, -1, 0, 0));
    ASSERT_TRUE(TestGetItemRange(-1, 1, 0, 0));

    //Empty range
    ASSERT_TRUE(TestGetItemRange(0, 0, 0, 0));
    ASSERT_TRUE(TestGetItemRange(5, 5, 0, 0));
    ASSERT_TRUE(TestGetItemRange(10, 10, 0, 0));
    ASSERT_TRUE(TestGetItemRange(21, 21, 0, 0));

    //Valid range
    ASSERT_TRUE(TestGetItemRange(0, 10, 0, 1));
    ASSERT_TRUE(TestGetItemRange(0, 30, 0, 3));
    ASSERT_TRUE(TestGetItemRange(0, 100, 0, 3));
    ASSERT_TRUE(TestGetItemRange(10, 20, 1, 1));
    ASSERT_TRUE(TestGetItemRange(11, 21, 1, 1));
    ASSERT_TRUE(TestGetItemRange(11, 24, 1, 2));
    ASSERT_TRUE(TestGetItemRange(20, 30, 1, 2));

    //Ouf of range
    ASSERT_TRUE(TestGetItemRange(33, 40, 0, 0));
    ASSERT_TRUE(TestGetItemRange(34, 50, 0, 0));
}


TEST_F(ListControlItemHeightManagerTest, GetItemRange_VariableHeight_Spacing) {

    item_source_->has_variable_item_heights = true;
    item_source_->item_spacing = 4;
    item_source_->item_count = 3;
    item_height_manager_->ReloadItemHeights();

    ASSERT_TRUE(TestGetItemRange(0, 5, 0, 1));
    ASSERT_TRUE(TestGetItemRange(0, 13, 0, 1));
    ASSERT_TRUE(TestGetItemRange(0, 14, 0, 1));
    ASSERT_TRUE(TestGetItemRange(0, 25, 0, 2));
    ASSERT_TRUE(TestGetItemRange(0, 26, 0, 2));
    ASSERT_TRUE(TestGetItemRange(0, 42, 0, 3));
    ASSERT_TRUE(TestGetItemRange(0, 60, 0, 3));

    ASSERT_TRUE(TestGetItemRange(10, 14, 1, 0));
    ASSERT_TRUE(TestGetItemRange(10, 16, 1, 1));

    ASSERT_TRUE(TestGetItemRange(16, 27, 1, 1));

    ASSERT_TRUE(TestGetItemRange(42, 50, 0, 0));
    ASSERT_TRUE(TestGetItemRange(43, 50, 0, 0));
}


TEST_F(ListControlItemHeightManagerTest, AddItems_FixedHeight) {

    item_source_->has_variable_item_heights = false;

    auto test_add_items = [this](
        std::size_t initial_count,
        std::size_t add_index,
        std::size_t add_count) {

        item_source_->item_count = initial_count;
        item_height_manager_->ReloadItemHeights();
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


TEST_F(ListControlItemHeightManagerTest, AddItems_VariableHeight_NoSpacing) {

    item_source_->has_variable_item_heights = true;

    auto test_add_items = [this](
        std::size_t initial_count, 
        std::size_t add_index,
        std::size_t add_count,
        const std::vector<float>& expected_item_heights) {

        item_source_->item_count = initial_count;
        item_height_manager_->ReloadItemHeights();
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


TEST_F(ListControlItemHeightManagerTest, AddItems_VariableHeight_Spacing) {

    auto test = [this](
        std::size_t initial_count,
        std::size_t add_index,
        std::size_t add_count,
        const std::vector<std::pair<float, float>>& expected) {

        item_source_->item_count = initial_count;
        item_height_manager_->ReloadItemHeights();
        item_source_->AddItems(add_index, add_count);
        return CheckItemPositionsAndHeights(expected);
    };

    item_source_->has_variable_item_heights = true;
    item_source_->item_spacing = 4;

    //Add items to empty item source
    ASSERT_TRUE(test(0, 0, 1, { { 0.f, 10.f } }));
    ASSERT_TRUE(test(0, 0, 3, { 
        { 0.f, 10.f },
        { 14.f, 11.f },
        { 29.f, 12.f },
    }));

    //Add items to head
    ASSERT_TRUE(test(3, 0, 3, {
        { 0.f, 10.f },
        { 14.f, 11.f },
        { 29.f, 12.f },
        { 45.f, 10.f },  //old
        { 59.f, 11.f }, //old
        { 74.f, 12.f }, //old
    }));

    //Add items to middle
    ASSERT_TRUE(test(5, 2, 3, {
        { 0.f, 10.f },  //old
        { 14.f, 11.f }, //old
        { 29.f, 12.f }, 
        { 45.f, 13.f },
        { 62.f, 14.f },
        { 80.f, 12.f }, //old
        { 96.f, 13.f }, //old
        { 113.f, 14.f }, //old
    }));

    //Add items to tail
    ASSERT_TRUE(test(3, 3, 3, {
        { 0.f, 10.f },  //old
        { 14.f, 11.f }, //old
        { 29.f, 12.f }, //old
        { 45.f, 13.f },
        { 62.f, 14.f },
        { 80.f, 15.f },
    }));
}


TEST_F(ListControlItemHeightManagerTest, RemoveItems_FixedHeight) {

    item_source_->has_variable_item_heights = false;

    auto test_remove_items = [this](
        std::size_t initial_count,
        std::size_t remove_index,
        std::size_t remove_count) {

        item_source_->item_count = initial_count;
        item_height_manager_->ReloadItemHeights();

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


TEST_F(ListControlItemHeightManagerTest, RemoveItems_VariableHeight_NoSpacing) {

    item_source_->has_variable_item_heights = true;

    auto test_remove_items = [this](
        std::size_t initial_count, 
        std::size_t remove_index, 
        std::size_t remove_count,
        const std::vector<float>& expected_item_heights) {

        item_source_->item_count = initial_count;
        item_height_manager_->ReloadItemHeights();

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


TEST_F(ListControlItemHeightManagerTest, RemoveItems_VariableHeight_Spacing) {

    auto test = [this](
        std::size_t initial_count,
        std::size_t remove_index,
        std::size_t remove_count,
        const std::vector<std::pair<float, float>>& expected) {

        item_source_->item_count = initial_count;
        item_height_manager_->ReloadItemHeights();
        item_source_->RemoveItems(remove_index, remove_count);
        return CheckItemPositionsAndHeights(expected);
    };

    item_source_->has_variable_item_heights = true;
    item_source_->item_spacing = 4;

    ASSERT_TRUE(test(5, 0, 2, {
        { 0.f, 12.f },
        { 16.f, 13.f },
        { 33.f, 14.f },
    }));

    ASSERT_TRUE(test(5, 2, 2, {
        { 0.f, 10.f },
        { 14.f, 11.f },
        { 29.f, 14.f },
    }));

    ASSERT_TRUE(test(5, 3, 2, {
        { 0.f, 10.f },
        { 14.f, 11.f },
        { 29.f, 12.f },
    }));
}


TEST_F(ListControlItemHeightManagerTest, UpdateItems_FixedHeight) {

    item_source_->has_variable_item_heights = false;

    auto test_update_items = [this](std::size_t initial_item_count, std::size_t update_index, std::size_t update_count) {

        item_source_->item_count = initial_item_count;
        item_height_manager_->ReloadItemHeights();

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


TEST_F(ListControlItemHeightManagerTest, UpdateItems_VariableHeight_NoSpacing) {

    item_source_->has_variable_item_heights = true;

    auto test_update_items = [this](
        std::size_t initial_item_count, 
        std::size_t update_index, 
        std::size_t update_count,
        const std::vector<float>& expected_item_heights) {

        item_source_->variable_item_height_adjustment = 0;
        item_source_->item_count = initial_item_count;
        item_height_manager_->ReloadItemHeights();

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


TEST_F(ListControlItemHeightManagerTest, UpdateItems_VariableHeight_Spacing) {

    auto test = [this](
        std::size_t initial_count,
        std::size_t update_index,
        std::size_t update_count,
        const std::vector<std::pair<float, float>>& expected) {

        item_source_->item_count = initial_count;
        item_source_->variable_item_height_adjustment = 0;
        item_height_manager_->ReloadItemHeights();

        item_source_->variable_item_height_adjustment = 2;
        item_source_->UpdateItems(update_index, update_count);
        return CheckItemPositionsAndHeights(expected);
    };

    item_source_->has_variable_item_heights = true;
    item_source_->item_spacing = 4;

    ASSERT_TRUE(test(5, 0, 5, {
        { 0.f, 12.f },
        { 16.f, 13.f }, 
        { 33.f, 14.f },
        { 51.f, 15.f },
        { 70.f, 16.f },
    }));

    ASSERT_TRUE(test(5, 0, 2, {
        { 0.f, 12.f },
        { 16.f, 13.f },
        { 33.f, 12.f },
        { 49.f, 13.f },
        { 66.f, 14.f },
    }));

    ASSERT_TRUE(test(5, 1, 2, {
        { 0.f, 10.f },
        { 14.f, 13.f },
        { 31.f, 14.f },
        { 49.f, 13.f },
        { 66.f, 14.f },
    }));

    ASSERT_TRUE(test(5, 3, 2, {
        { 0.f, 10.f },
        { 14.f, 11.f },
        { 29.f, 12.f },
        { 45.f, 15.f },
        { 64.f, 16.f },
    }));
}