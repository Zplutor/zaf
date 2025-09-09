#include <gtest/gtest.h>
#include <zaf/control/list_control.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include <zaf/creation.h>
#include <zaf/dynamic/boxing/numeric.h>

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

    std::shared_ptr<dynamic::Object> GetDataAtIndex(std::size_t index) const override {
        return data_list_[index];
    }

private:
    std::vector<std::shared_ptr<Int32>> data_list_;
};


class TestDelegate : public ListControlDelegate {
public:
    float EstimateItemHeight(
        std::size_t item_index, 
        const std::shared_ptr<dynamic::Object>& item_data) override {

        return 10;
    }
};


class TestListControl : public ListControl {
protected:
    void Initialize() override {

        __super::Initialize();

        SetBorder({});
        SetPadding({});
        SetHeight(100);

        data_source_ = Create<TestDataSource>();
        delegate_ = Create<TestDelegate>();
        SetDataSource(data_source_);
        SetDelegate(delegate_);
    }

private:
    std::shared_ptr<TestDataSource> data_source_;
    std::shared_ptr<TestDelegate> delegate_;
};

}

TEST(ListControlTest, DefaultSelectionMode) {

    auto list = Create<TestListControl>();
    ASSERT_EQ(list->SelectionMode(), ListSelectionMode::Single);
}


TEST(ListControlTest, SetSelectionMode) {

    auto list = Create<TestListControl>();
    std::size_t selection_changed_count{};
    auto sub = list->SelectionChangedEvent().Subscribe(std::bind([&selection_changed_count]() {
        selection_changed_count++;
    }));

    //Change selection mode to none while there is no selection.
    {
        list->SetSelectionMode(ListSelectionMode::Single);
        list->UnselectAllItems();
        selection_changed_count = 0;
        list->SetSelectionMode(ListSelectionMode::None);
        ASSERT_EQ(selection_changed_count, 0);
    }

    //Change selection mode to none while there is selection.
    {
        list->SetSelectionMode(ListSelectionMode::Single);
        list->SelectItemAtIndex(6);
        selection_changed_count = 0;
        list->SetSelectionMode(ListSelectionMode::None);
        ASSERT_EQ(selection_changed_count, 1);
        ASSERT_EQ(list->SelectedItemCount(), 0);
    }

    //Change selection mode to single while there is no selection.
    {
        list->SetSelectionMode(ListSelectionMode::Single);
        list->UnselectAllItems();
        selection_changed_count = 0;
        list->SetSelectionMode(ListSelectionMode::Single);
        ASSERT_EQ(selection_changed_count, 0);
    }

    //Change selection mode to single while there is one selection.
    {
        list->SetSelectionMode(ListSelectionMode::Single);
        list->SelectItemAtIndex(7);
        selection_changed_count = 0;
        list->SetSelectionMode(ListSelectionMode::Single);
        ASSERT_EQ(selection_changed_count, 0);
        ASSERT_EQ(list->FirstSelectedItemIndex(), 7);
    }

    //Change selection mode to single while there is multiple selection.
    {
        list->SetSelectionMode(ListSelectionMode::SimpleMultiple);
        list->SelectItemAtIndex(7);
        list->SelectItemAtIndex(8);
        list->SelectItemAtIndex(4);
        selection_changed_count = 0;
        list->SetSelectionMode(ListSelectionMode::Single);
        ASSERT_EQ(selection_changed_count, 1);
        ASSERT_EQ(list->FirstSelectedItemIndex(), 4);
    }

    //Change selection mode to multiple while there is one selection.
    {
        list->SetSelectionMode(ListSelectionMode::Single);
        list->SelectItemAtIndex(3);
        selection_changed_count = 0;
        list->SetSelectionMode(ListSelectionMode::SimpleMultiple);
        ASSERT_EQ(selection_changed_count, 0);
        ASSERT_EQ(list->SelectedItemCount(), 1);

        list->SetSelectionMode(ListSelectionMode::ExtendedMultiple);
        ASSERT_EQ(selection_changed_count, 0);
        ASSERT_EQ(list->SelectedItemCount(), 1);
    }

    //Change selection mode to multiple while there multiple selection.
    {
        list->SetSelectionMode(ListSelectionMode::SimpleMultiple);
        list->SelectItemAtIndex(3);
        list->SelectItemAtIndex(2);
        list->SelectItemAtIndex(0);
        selection_changed_count = 0;
        list->SetSelectionMode(ListSelectionMode::ExtendedMultiple);
        ASSERT_EQ(selection_changed_count, 0);
        ASSERT_EQ(list->SelectedItemCount(), 3);
    }
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

    for (auto index : Range(0, list->ItemCount())) {
        auto visible_item = list->GetVisibleItemAtIndex(index);
        ASSERT_TRUE(visible_item->IsSelected());
    }

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

    for (auto index : Range(0, list->ItemCount())) {
        auto visible_item = list->GetVisibleItemAtIndex(index);
        ASSERT_FALSE(visible_item->IsSelected());
    }
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
        list->SetSelectionMode(ListSelectionMode::None);
        list->SelectItemAtIndex(1);
        list->SelectItemAtIndex(3);
        ASSERT_EQ(list->SelectedItemCount(), 0);
        ASSERT_EQ(selection_changed_count, 0);
    }

    //Select item in single selection mode.
    {
        selection_changed_count = 0;
        list->SetSelectionMode(ListSelectionMode::Single);
        list->SelectItemAtIndex(0);
        list->SelectItemAtIndex(5);
        list->SelectItemAtIndex(4);
        list->SelectItemAtIndex(4); //This call won't raise selection changed event.
        ASSERT_EQ(list->SelectedItemCount(), 1);
        ASSERT_EQ(list->FirstSelectedItemIndex(), 4);
        ASSERT_TRUE(list->GetVisibleItemAtIndex(4)->IsSelected());
        ASSERT_EQ(selection_changed_count, 3);
    }

    //Select item in simple multiple selection mode.
    {
        list->UnselectAllItems();
        list->SetSelectionMode(ListSelectionMode::SimpleMultiple);
        selection_changed_count = 0;
        list->SelectItemAtIndex(1);
        list->SelectItemAtIndex(2);
        list->SelectItemAtIndex(9);
        list->SelectItemAtIndex(4);
        std::vector<std::size_t> expected_selected_indexes{ 1, 2, 4, 9 };
        ASSERT_EQ(list->SelectedItemIndexes(), expected_selected_indexes);
        for (auto index : expected_selected_indexes) {
            ASSERT_TRUE(list->GetVisibleItemAtIndex(index)->IsSelected());
        }
        ASSERT_EQ(selection_changed_count, 4);
    }

    //Select item in extended multiple selection mode.
    {
        list->UnselectAllItems();
        list->SetSelectionMode(ListSelectionMode::ExtendedMultiple);
        selection_changed_count = 0;
        list->SelectItemAtIndex(3);
        list->SelectItemAtIndex(8);
        std::vector<std::size_t> expected_selected_indexes{ 3, 8 };
        ASSERT_EQ(list->SelectedItemIndexes(), expected_selected_indexes);
        for (auto index : expected_selected_indexes) {
            ASSERT_TRUE(list->GetVisibleItemAtIndex(index)->IsSelected());
        }
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
        list->SetSelectionMode(ListSelectionMode::Single);
        list->SelectItemAtIndex(1);
        selection_changed_count = 0;

        list->UnselectItemAtIndex(0);
        ASSERT_EQ(selection_changed_count, 0);
        ASSERT_EQ(list->FirstSelectedItemIndex(), 1);

        list->UnselectItemAtIndex(1);
        ASSERT_EQ(selection_changed_count, 1);
        ASSERT_EQ(list->SelectedItemCount(), 0);
        ASSERT_FALSE(list->GetVisibleItemAtIndex(1)->IsSelected());
    }

    //Unselect item in multiple selection mode.
    {
        list->SetSelectionMode(ListSelectionMode::ExtendedMultiple);
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