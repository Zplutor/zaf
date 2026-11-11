#include <memory>
#include <set>
#include <gtest/gtest.h>
#include <zaf/internal/list/list_selection_store.h>

class ListSelectionStoreTest : public testing::Test {
public:
    void SetUp() override {

        selection_store_ = std::make_shared<zaf::internal::ListSelectionStore>();
    }

protected:
    void CheckSelection(
        std::size_t begin_check_index, 
        std::size_t check_index_count, 
        const std::set<std::size_t>& expected_selected_indexes) {

        for (std::size_t index = begin_check_index;
             index < begin_check_index + check_index_count;
             ++index) {

            bool is_selected = selection_store_->IsIndexSelected(index);

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
    std::shared_ptr<zaf::internal::ListSelectionStore> selection_store_;
};


TEST_F(ListSelectionStoreTest, ReplaceSelection) {
    
    auto test = [this](
        std::size_t replace_index, 
        std::size_t replace_count,
        bool expected_is_changed,
        std::size_t check_index,
        std::size_t check_count,
        const std::set<std::size_t>& expected_selected_indexes) {

        bool is_changed = selection_store_->ReplaceSelection(replace_index, replace_count);
        ASSERT_EQ(is_changed, expected_is_changed);
        CheckSelection(check_index, check_count, expected_selected_indexes);
    };

    //Empty ranges.
    test(0, 0, false, 0, 10, { });
    test(3, 0, false, 0, 10, { });

    //Single selection
    test(0, 1, true, 0, 10, { 0 });
    test(0, 1, false, 0, 10, { 0 });
    test(2, 1, true, 0, 10, { 2 });
    test(2, 1, false, 0, 10, { 2 });

    //Multiple selection
    test(0, 5, true, 0, 10, { 0, 1, 2, 3, 4 });
    test(0, 5, false, 0, 10, { 0, 1, 2, 3, 4 });
    test(3, 5, true, 0, 10, { 3, 4, 5, 6, 7 });
    test(3, 5, false, 0, 10, { 3, 4, 5, 6, 7 });
    test(11, 3, true, 0, 20, { 11, 12, 13 });
    test(11, 3, false, 0, 20, { 11, 12, 13 });

    //A selection that is not a single range is always replaced.
    selection_store_->AddSelection(20, 2);
    test(11, 3, true, 0, 30, { 11, 12, 13 });
}


TEST_F(ListSelectionStoreTest, RevertSelection) {

    auto test = [this](
        std::size_t revert_index,
        std::size_t check_index,
        std::size_t check_count,
        const std::set<std::size_t>& expected_selected_indexes) {

        selection_store_->RevertSelection(revert_index);
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
    selection_store_->ReplaceSelection(7, 2);
    test(7, 0, 10, { 8 });
    test(8, 0, 10, { });
}


TEST_F(ListSelectionStoreTest, AddSelection) {

    auto test = [this](
        std::size_t add_index,
        std::size_t add_count,
        bool expected_is_changed,
        std::size_t check_index,
        std::size_t check_count,
        const std::set<std::size_t>& expected_selected_indexes) {

        bool is_changed = selection_store_->AddSelection(add_index, add_count);
        ASSERT_EQ(is_changed, expected_is_changed);
        CheckSelection(check_index, check_count, expected_selected_indexes);
    };

    //Add to empty selection
    selection_store_->ReplaceSelection(0, 0);
    test(0, 1, true, 0, 10, { 0 });
    test(0, 4, true, 0, 10, { 0, 1, 2, 3 });

    //Add empty selection
    selection_store_->ReplaceSelection(0, 0);
    test(0, 0, false, 0, 10, { });

    //Add selection that doesn't intersect with other.
    selection_store_->ReplaceSelection(10, 2);
    test(0, 2, true, 0, 20, { 0, 1, 10, 11 });
    test(9, 1, true, 0, 20, { 0, 1, 9, 10, 11 });
    test(12, 1, true, 0, 20, { 0, 1, 9, 10, 11, 12 });
    test(15, 3, true, 0, 20, { 0, 1, 9, 10, 11, 12, 15, 16, 17 });

    //Add selection intersects with other.
    selection_store_->ReplaceSelection(10, 2);
    test(8, 3, true, 0, 20, { 8, 9, 10, 11 });
    test(10, 4, true, 0, 20, { 8, 9, 10, 11, 12, 13 });
    test(6, 10, true, 0, 20, { 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 });
    test(9, 3, false, 0, 20, { 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 });

    //Add selection across other selections.
    selection_store_->ReplaceSelection(5, 2);
    selection_store_->RevertSelection(9);
    selection_store_->RevertSelection(10);
    test(4, 8, true, 0, 20, { 4, 5, 6, 7, 8, 9, 10, 11 });
    selection_store_->RevertSelection(5);
    test(0, 0, false, 0, 20, { 4, 6, 7, 8, 9, 10, 11 });
    selection_store_->RevertSelection(6);
    test(0, 0, false, 0, 20, { 4, 7, 8, 9, 10, 11 });
    selection_store_->RevertSelection(9);
    test(0, 0, false, 0, 20, { 4, 7, 8, 10, 11 });
    selection_store_->RevertSelection(10);
    test(0, 0, false, 0, 20, { 4, 7, 8, 11 });
}


TEST_F(ListSelectionStoreTest, RemoveSelection) {

    auto test = [this](
        std::size_t remove_index,
        std::size_t remove_count,
        bool expected_is_changed,
        std::size_t check_index,
        std::size_t check_count,
        const std::set<std::size_t>& expected_selected_indexes) {

        bool is_changed = selection_store_->RemoveSelection(remove_index, remove_count);
        ASSERT_EQ(is_changed, expected_is_changed);
        CheckSelection(check_index, check_count, expected_selected_indexes);
    };

    //Remove from empty selection.
    selection_store_->ReplaceSelection(0, 0);
    test(0, 10, false, 0, 10, { });

    //Remove empty selection.
    selection_store_->ReplaceSelection(0, 5);
    test(0, 0, false, 0, 10, { 0, 1, 2, 3, 4 });

    //Remove before selection.
    selection_store_->ReplaceSelection(5, 2);
    test(0, 5, false, 0, 10, { 5, 6 });

    //Remove after selection.
    selection_store_->ReplaceSelection(5, 2);
    test(7, 5, false, 0, 10, { 5, 6 });

    //Remove selection.
    selection_store_->ReplaceSelection(10, 10);
    test(0, 12, true, 0, 30, { 12, 13, 14, 15, 16, 17, 18, 19 });
    test(18, 5, true, 0, 30, { 12, 13, 14, 15, 16, 17 });
    test(14, 2, true, 0, 30, { 12, 13, 16, 17 });

    //Remove whole selection.
    selection_store_->ReplaceSelection(10, 2);
    test(10, 2, true, 0, 20, { });
    test(8, 4, false, 0, 20, { });
    test(10, 4, false, 0, 20, { });
    test(8, 8, false, 0, 20, { });

    //Remove selection across multi selection.
    selection_store_->ReplaceSelection(4, 2);
    selection_store_->AddSelection(8, 2);
    test(0, 10, true, 0, 10, { });

    selection_store_->ReplaceSelection(4, 2);
    selection_store_->AddSelection(8, 2);
    test(5, 4, true, 0, 10, { 4, 9 });
}


TEST_F(ListSelectionStoreTest, AdjustSelectionByAddingIndexes) {

    auto test = [this](
        std::size_t add_index,
        std::size_t add_count,
        std::size_t check_index,
        std::size_t check_count, 
        const std::set<std::size_t>& expected_selected_indexes) {

        selection_store_->AdjustSelectionByAddingIndexes(add_index, add_count);
        CheckSelection(check_index, check_count, expected_selected_indexes);
    };

    //Add indexes to empty selection.
    selection_store_->ReplaceSelection(0, 0);
    test(0, 0, 0, 10, { });
    test(0, 1, 0, 10, { });
    test(3, 0, 0, 10, { });
    test(3, 2, 0, 10, { });

    //Add indexes before selection.
    selection_store_->RevertSelection(0);
    selection_store_->RevertSelection(5);
    test(0, 0, 0, 10, { 0, 5 });
    test(0, 1, 0, 10, { 1, 6 });
    test(1, 1, 0, 10, { 2, 7 });
    test(0, 4, 0, 20, { 6, 11 });

    //Add indexes to the middle of selection.
    selection_store_->ReplaceSelection(0, 0);
    selection_store_->RevertSelection(0);
    selection_store_->RevertSelection(5);
    selection_store_->RevertSelection(6);
    selection_store_->RevertSelection(10);
    test(6, 2, 0, 20, { 0, 5, 8, 12 });

    //Insert indexes into selection.
    selection_store_->ReplaceSelection(0, 10);
    test(3, 2, 0, 20, { 0, 1, 2, 5, 6, 7, 8, 9, 10, 11 });
    ASSERT_EQ(0, selection_store_->GetFirstSelectedIndex());

    //Add indexes after selection.
    selection_store_->ReplaceSelection(0, 0);
    selection_store_->RevertSelection(0);
    selection_store_->RevertSelection(5);
    test(1, 1, 0, 10, { 0, 6 });
    test(1, 3, 0, 10, { 0, 9 });
}


TEST_F(ListSelectionStoreTest, AdjustSelectionByRemovingIndexes) {

    auto test = [this](
        std::size_t remove_index,
        std::size_t remove_count,
        std::size_t check_index,
        std::size_t check_count,
        const std::set<std::size_t>& expected_selected_indexes) {

        selection_store_->AdjustSelectionByRemovingIndexes(remove_index, remove_count);
        CheckSelection(check_index, check_count, expected_selected_indexes);
    };

    //Remove from empty selection.
    selection_store_->ReplaceSelection(0, 0);
    test(0, 0, 0, 10, { });
    test(0, 3, 0, 10, { });

    //Remove indexes before selection.
    selection_store_->ReplaceSelection(0, 0);
    selection_store_->RevertSelection(1);
    selection_store_->RevertSelection(4);
    test(0, 1, 0, 10, { 0, 3 });
    test(1, 2, 0, 10, { 0, 1 });

    //Remove indexes at the middle of selection.
    selection_store_->ReplaceSelection(5, 5);
    test(0, 11, 0, 20, { });
    selection_store_->ReplaceSelection(5, 5);
    test(0, 7, 0, 20, { 0, 1, 2 });
    selection_store_->ReplaceSelection(5, 5);
    test(5, 2, 0, 20, { 5, 6, 7 });
    selection_store_->ReplaceSelection(5, 5);
    test(6, 3, 0, 20, { 5, 6 });
    selection_store_->ReplaceSelection(5, 5);
    test(7, 3, 0, 20, { 5, 6 });
    selection_store_->ReplaceSelection(5, 5);
    test(7, 10, 0, 20, { 5, 6 });

    //Remove indexes after selection.
    selection_store_->ReplaceSelection(0, 0);
    selection_store_->RevertSelection(0);
    selection_store_->RevertSelection(5);
    test(6, 1, 0, 10, { 0, 5 });
    test(3, 2, 0, 10, { 0, 3 });
}