#include <gtest/gtest.h>
#include <zaf/control/list_control.h>
#include <zaf/control/list_data_source.h>
#include <zaf/creation.h>
#include <zaf/object/boxing/numeric.h>

using namespace zaf;

namespace {

class TestDataSource : public ListDataSource {
public:
    TestDataSource() {
        data_list_ = {
            Box(0),
            Box(1),
            Box(2),
            Box(3),
            Box(4),
            Box(5),
            Box(6),
            Box(7),
            Box(8),
            Box(9),
        };
    }

    std::size_t GetDataCount() const override {
        return data_list_.size();
    }

    std::shared_ptr<Object> GetDataAtIndex(std::size_t index) const override {
        return data_list_[index];
    }

private:
    std::vector<std::shared_ptr<Int32>> data_list_;
};


class TestListControl : public ListControl {
protected:
    void Initialize() override {

        __super::Initialize();

        data_source_ = Create<TestDataSource>();
        SetDataSource(data_source_);
    }

private:
    std::shared_ptr<TestDataSource> data_source_;
};

}

TEST(ListControlTest, DefaultSelectionMode) {

    auto list = Create<TestListControl>();
    ASSERT_EQ(list->SelectionMode(), SelectionMode::Single);
}


TEST(ListControlTest, SelectAllItems) {

    auto list = Create<TestListControl>();

    std::size_t selection_changed_count{};
    auto sub = list->SelectionChangedEvent().Subscribe(std::bind([&selection_changed_count]() {
        selection_changed_count++;
    }));

    list->SelectAllItems();

    ASSERT_EQ(list->SelectedItemCount(), 10);
    ASSERT_EQ(selection_changed_count, 1);

    //Selection won't changed if all items are already selected.
    list->SelectAllItems();
    ASSERT_EQ(list->SelectedItemCount(), 10);
    ASSERT_EQ(selection_changed_count, 1);
}


TEST(ListControlTest, UnselectAllItems) {

    auto list = Create<TestListControl>();

    std::size_t selection_changed_count{};
    auto sub = list->SelectionChangedEvent().Subscribe(std::bind([&selection_changed_count]() {
        selection_changed_count++;
    }));

    list->UnselectAllItems();

    //Selection won't changed if no item is selected.
    ASSERT_EQ(list->SelectedItemCount(), 0);
    ASSERT_EQ(selection_changed_count, 0);

    list->SelectAllItems();
    list->UnselectAllItems();
    ASSERT_EQ(list->SelectedItemCount(), 0);
    ASSERT_EQ(selection_changed_count, 2);
}


TEST(ListControlTest, SelectItemAtIndex) {

    auto list = Create<TestListControl>();

    std::size_t selection_changed_count{};
    auto sub = list->SelectionChangedEvent().Subscribe(std::bind([&selection_changed_count]() {
        selection_changed_count++;
    }));

    //Exception will be throw if index is out of bounds.
    {
        ASSERT_THROW(list->SelectItemAtIndex(10), PreconditionError);
    }

    //Select item in none selection mode.
    {
        list->SetSelectionMode(SelectionMode::None);
        list->SelectItemAtIndex(1);
        list->SelectItemAtIndex(3);
        ASSERT_EQ(list->SelectedItemCount(), 0);
        ASSERT_EQ(selection_changed_count, 0);
    }

    //Select item in single selection mode.
    {
        selection_changed_count = 0;
        list->SetSelectionMode(SelectionMode::Single);
        list->SelectItemAtIndex(0);
        list->SelectItemAtIndex(5);
        list->SelectItemAtIndex(4);
        list->SelectItemAtIndex(4); //This call won't raise selection changed event.
        ASSERT_EQ(list->SelectedItemCount(), 1);
        ASSERT_EQ(list->FirstSelectedItemIndex(), 4);
        ASSERT_EQ(selection_changed_count, 3);
    }

    //Select item in simple multiple selection mode.
    {
        list->UnselectAllItems();
        list->SetSelectionMode(SelectionMode::SimpleMultiple);
        selection_changed_count = 0;
        list->SelectItemAtIndex(1);
        list->SelectItemAtIndex(2);
        list->SelectItemAtIndex(9);
        list->SelectItemAtIndex(4);
        std::vector<std::size_t> expected_selected_indexes{ 1, 2, 4, 9 };
        ASSERT_EQ(list->SelectedItemIndexes(), expected_selected_indexes);
        ASSERT_EQ(selection_changed_count, 4);
    }

    //Select item in extended multiple selection mode.
    {
        list->UnselectAllItems();
        list->SetSelectionMode(SelectionMode::ExtendedMultiple);
        selection_changed_count = 0;
        list->SelectItemAtIndex(3);
        list->SelectItemAtIndex(8);
        std::vector<std::size_t> expected_selected_indexes{ 3, 8 };
        ASSERT_EQ(list->SelectedItemIndexes(), expected_selected_indexes);
        ASSERT_EQ(selection_changed_count, 2);
    }
}


TEST(ListControlTest, UnselectItemAtIndex) {

    auto list = Create<TestListControl>();

    std::size_t selection_changed_count{};
    auto sub = list->SelectionChangedEvent().Subscribe(std::bind([&selection_changed_count]() {
        selection_changed_count++;
    }));

    //Exception will be throw if index is out of bounds.
    {
        ASSERT_THROW(list->UnselectItemAtIndex(10), PreconditionError);
    }

    //Unselect item in single selection mode.
    {
        list->SetSelectionMode(SelectionMode::Single);
        list->SelectItemAtIndex(1);
        selection_changed_count = 0;

        list->UnselectItemAtIndex(0);
        ASSERT_EQ(selection_changed_count, 0);
        ASSERT_EQ(list->FirstSelectedItemIndex(), 1);

        list->UnselectItemAtIndex(1);
        ASSERT_EQ(selection_changed_count, 1);
        ASSERT_EQ(list->SelectedItemCount(), 0);
    }

    //Unselect item in multiple selection mode.
    {
        list->SetSelectionMode(SelectionMode::ExtendedMultiple);
        list->SelectItemAtIndex(1);
        list->SelectItemAtIndex(2);
        list->SelectItemAtIndex(3);
        selection_changed_count = 0;

        list->UnselectItemAtIndex(2);
        ASSERT_EQ(selection_changed_count, 1);
        std::vector<std::size_t> expected_selected_indexes{ 1, 3 };
        ASSERT_EQ(list->SelectedItemIndexes(), expected_selected_indexes);

        list->UnselectItemAtIndex(3);
        ASSERT_EQ(selection_changed_count, 2);
        expected_selected_indexes = { 1 };
        ASSERT_EQ(list->SelectedItemIndexes(), expected_selected_indexes);

        list->UnselectItemAtIndex(1);
        ASSERT_EQ(selection_changed_count, 3);
        expected_selected_indexes = {};
        ASSERT_EQ(list->SelectedItemIndexes(), expected_selected_indexes);
    }
}