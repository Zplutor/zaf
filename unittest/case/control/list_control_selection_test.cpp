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