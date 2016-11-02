#include <memory>
#include <set>
#include <gtest/gtest.h>
#include <zaf/control/internal/list_control_item_selection_manager.h>

class ListControlItemSelectionManagerTest : public testing::Test {
public:
    void SetUp() override {

        item_selection_manager_ = std::make_shared<zaf::internal::ListControlItemSelectionManager>();
    }

protected:
    void CheckSelection(
        std::size_t begin_check_index, 
        std::size_t check_index_count, 
        const std::set<std::size_t>& expected_selected_indexes) {

        for (std::size_t index = begin_check_index;
             index < begin_check_index + check_index_count;
             ++index) {

            bool is_selected = item_selection_manager_->IsIndexSelected(index);

            auto iterator = expected_selected_indexes.find(index);
            if (iterator == expected_selected_indexes.end()) {
                ASSERT_FALSE(is_selected);
            }
            else {
                ASSERT_TRUE(is_selected);
            }
        }
    }

protected:
    std::shared_ptr<zaf::internal::ListControlItemSelectionManager> item_selection_manager_;
};


TEST_F(ListControlItemSelectionManagerTest, ReplaceSelection) {
    
    auto test = [this](
        std::size_t replace_index, 
        std::size_t replace_count,
        std::size_t check_index,
        std::size_t check_count,
        const std::set<std::size_t>& expected_selected_indexes) {

        item_selection_manager_->ReplaceSelection(replace_index, replace_count);
        CheckSelection(check_index, check_count, expected_selected_indexes);
    };

    //Empty range.
    test(0, 0, 0, 10, { });
    test(3, 0, 0, 10, { });

    //Single selection
    test(0, 1, 0, 10, { 0 });
    test(2, 1, 0, 10, { 2 });

    //Multiple selection
    test(0, 5, 0, 10, { 0, 1, 2, 3, 4 });
    test(3, 5, 0, 10, { 3, 4, 5, 6, 7 });
    test(11, 3, 0, 20, { 11, 12, 13 });
}


TEST_F(ListControlItemSelectionManagerTest, RevertSelection) {

    auto test = [this](
        std::size_t revert_index,
        std::size_t check_index,
        std::size_t check_count,
        const std::set<std::size_t>& expected_selected_indexes) {

        item_selection_manager_->RevertSelection(revert_index);
        CheckSelection(check_index, check_count, expected_selected_indexes);
    };

    test(0, 0, 10, { 0 });
    test(0, 0, 10, { });
    test(0, 0, 10, { 0 });
    test(0, 0, 10, { });

    test(33, 30, 35, { 33 });
    test(34, 30, 35, { 33, 34 });
    test(3, 0, 40, { 3, 33, 34 });

    //Revert in selection range
    item_selection_manager_->ReplaceSelection(7, 2);
    test(7, 0, 10, { 8 });
    test(8, 0, 10, { });
}


TEST_F(ListControlItemSelectionManagerTest, AddSelection) {

    auto test = [this](
        std::size_t add_index,
        std::size_t add_count,
        std::size_t check_index,
        std::size_t check_count,
        const std::set<std::size_t>& expected_selected_indexes) {

        item_selection_manager_->AddSelection(add_index, add_count);
        CheckSelection(check_index, check_count, expected_selected_indexes);
    };

    //Add to empty selection
    item_selection_manager_->ReplaceSelection(0, 0);
    test(0, 1, 0, 10, { 0 });
    test(0, 4, 0, 10, { 0, 1, 2, 3 });

    //Add empty selection
    item_selection_manager_->ReplaceSelection(0, 0);
    test(0, 0, 0, 10, { });

    //Add selection that doesn't intersect with other.
    item_selection_manager_->ReplaceSelection(10, 2);
    test(0, 2, 0, 20, { 0, 1, 10, 11 });
    test(9, 1, 0, 20, { 0, 1, 9, 10, 11 });
    test(12, 1, 0, 20, { 0, 1, 9, 10, 11, 12 });
    test(15, 3, 0, 20, { 0, 1, 9, 10, 11, 12, 15, 16, 17 });

    //Add selection intersects with other.
    item_selection_manager_->ReplaceSelection(10, 2);
    test(8, 3, 0, 20, { 8, 9, 10, 11 });
    test(10, 4, 0, 20, { 8, 9, 10, 11, 12, 13 });
    test(6, 10, 0, 20, { 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 });
    test(9, 3, 0, 20, { 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 });

    //Add selection across other selections.
    item_selection_manager_->ReplaceSelection(5, 2);
    item_selection_manager_->RevertSelection(9);
    item_selection_manager_->RevertSelection(10);
    test(4, 8, 0, 20, { 4, 5, 6, 7, 8, 9, 10, 11 });
    item_selection_manager_->RevertSelection(5);
    test(0, 0, 0, 20, { 4, 6, 7, 8, 9, 10, 11 });
    item_selection_manager_->RevertSelection(6);
    test(0, 0, 0, 20, { 4, 7, 8, 9, 10, 11 });
    item_selection_manager_->RevertSelection(9);
    test(0, 0, 0, 20, { 4, 7, 8, 10, 11 });
    item_selection_manager_->RevertSelection(10);
    test(0, 0, 0, 20, { 4, 7, 8, 11 });
}


TEST_F(ListControlItemSelectionManagerTest, RemoveSelection) {

    auto test = [this](
        std::size_t remove_index,
        std::size_t remove_count,
        std::size_t check_index,
        std::size_t check_count,
        const std::set<std::size_t>& expected_selected_indexes) {

        item_selection_manager_->RemoveSelection(remove_index, remove_count);
        CheckSelection(check_index, check_count, expected_selected_indexes);
    };

    //Remove from empty selection.
    item_selection_manager_->ReplaceSelection(0, 0);
    test(0, 10, 0, 10, { });

    //Remove empty selection.
    item_selection_manager_->ReplaceSelection(0, 5);
    test(0, 0, 0, 10, { 0, 1, 2, 3, 4 });

    //Remove before selection.
    item_selection_manager_->ReplaceSelection(5, 2);
    test(0, 5, 0, 10, { 5, 6 });

    //Remove after selection.
    item_selection_manager_->ReplaceSelection(5, 2);
    test(7, 5, 0, 10, { 5, 6 });

    //Remove selection.
    item_selection_manager_->ReplaceSelection(10, 10);
    test(0, 12, 0, 30, { 12, 13, 14, 15, 16, 17, 18, 19 });
    test(18, 5, 0, 30, { 12, 13, 14, 15, 16, 17 });
    test(14, 2, 0, 30, { 12, 13, 16, 17 });

    //Remove whole selection.
    item_selection_manager_->ReplaceSelection(10, 2);
    test(10, 2, 0, 20, { });
    test(8, 4, 0, 20, { });
    test(10, 4, 0, 20, { });
    test(8, 8, 0, 20, { });

    //Remove selection across multi selection.
    item_selection_manager_->ReplaceSelection(4, 2);
    item_selection_manager_->AddSelection(8, 2);
    test(0, 10, 0, 10, { });

    item_selection_manager_->ReplaceSelection(4, 2);
    item_selection_manager_->AddSelection(8, 2);
    test(5, 4, 0, 10, { 4, 9 });
}


TEST_F(ListControlItemSelectionManagerTest, AdjustSelectionByAddingIndexes) {

    auto test = [this](
        std::size_t add_index,
        std::size_t add_count,
        std::size_t check_index,
        std::size_t check_count, 
        const std::set<std::size_t>& expected_selected_indexes) {

        item_selection_manager_->AdjustSelectionByAddingIndexes(add_index, add_count);
        CheckSelection(check_index, check_count, expected_selected_indexes);
    };

    //Add indexes to empty selection.
    item_selection_manager_->ReplaceSelection(0, 0);
    test(0, 0, 0, 10, { });
    test(0, 1, 0, 10, { });
    test(3, 0, 0, 10, { });
    test(3, 2, 0, 10, { });

    //Add indexes before selection.
    item_selection_manager_->RevertSelection(0);
    item_selection_manager_->RevertSelection(5);
    test(0, 0, 0, 10, { 0, 5 });
    test(0, 1, 0, 10, { 1, 6 });
    test(1, 1, 0, 10, { 2, 7 });
    test(0, 4, 0, 20, { 6, 11 });

    //Add indexes to the middle of selection.
    item_selection_manager_->ReplaceSelection(0, 0);
    item_selection_manager_->RevertSelection(0);
    item_selection_manager_->RevertSelection(5);
    item_selection_manager_->RevertSelection(6);
    item_selection_manager_->RevertSelection(10);
    test(6, 2, 0, 20, { 0, 5, 8, 12 });

    //Insert indexes into selection.
    item_selection_manager_->ReplaceSelection(0, 10);
    test(3, 2, 0, 20, { 0, 1, 2, 5, 6, 7, 8, 9, 10, 11 });
    ASSERT_EQ(0, item_selection_manager_->GetFirstSelectedIndex());

    //Add indexes after selection.
    item_selection_manager_->ReplaceSelection(0, 0);
    item_selection_manager_->RevertSelection(0);
    item_selection_manager_->RevertSelection(5);
    test(1, 1, 0, 10, { 0, 6 });
    test(1, 3, 0, 10, { 0, 9 });
}


TEST_F(ListControlItemSelectionManagerTest, AdjustSelectionByRemovingIndexes) {

    auto test = [this](
        std::size_t remove_index,
        std::size_t remove_count,
        std::size_t check_index,
        std::size_t check_count,
        const std::set<std::size_t>& expected_selected_indexes) {

        item_selection_manager_->AdjustSelectionByRemovingIndexes(remove_index, remove_count);
        CheckSelection(check_index, check_count, expected_selected_indexes);
    };

    //Remove from empty selection.
    item_selection_manager_->ReplaceSelection(0, 0);
    test(0, 0, 0, 10, { });
    test(0, 3, 0, 10, { });

    //Remove indexes before selection.
    item_selection_manager_->ReplaceSelection(0, 0);
    item_selection_manager_->RevertSelection(1);
    item_selection_manager_->RevertSelection(4);
    test(0, 1, 0, 10, { 0, 3 });
    test(1, 2, 0, 10, { 0, 1 });

    //Remove indexes at the middle of selection.
    item_selection_manager_->ReplaceSelection(5, 5);
    test(0, 11, 0, 20, { });
    item_selection_manager_->ReplaceSelection(5, 5);
    test(0, 7, 0, 20, { 0, 1, 2 });
    item_selection_manager_->ReplaceSelection(5, 5);
    test(5, 2, 0, 20, { 5, 6, 7 });
    item_selection_manager_->ReplaceSelection(5, 5);
    test(6, 3, 0, 20, { 5, 6 });
    item_selection_manager_->ReplaceSelection(5, 5);
    test(7, 3, 0, 20, { 5, 6 });
    item_selection_manager_->ReplaceSelection(5, 5);
    test(7, 10, 0, 20, { 5, 6 });

    //Remove indexes after selection.
    item_selection_manager_->ReplaceSelection(0, 0);
    item_selection_manager_->RevertSelection(0);
    item_selection_manager_->RevertSelection(5);
    test(6, 1, 0, 10, { 0, 5 });
    test(3, 2, 0, 10, { 0, 3 });
}