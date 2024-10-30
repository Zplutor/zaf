#include <gtest/gtest.h>
#include <zaf/control/list_control.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include <zaf/control/list_item_container.h>

using namespace zaf;

namespace {

class DataSource : public ListDataSource {
public:
    DataSource() {

        data_ = {
            Box(std::int32_t(1)),
            Box(std::int32_t(2)),
            Box(std::int32_t(3)),
            Box(std::int32_t(4)),
            Box(std::int32_t(5)),
        };
    }

    std::size_t GetDataCount() const override {
        return data_.size();
    }

    std::shared_ptr<Object> GetDataAtIndex(std::size_t index) const override {
        return data_[index];
    }

    void Move(std::size_t from, std::size_t to) {

        auto data = data_[from];
        data_.erase(std::next(data_.begin(), from));
        data_.insert(std::next(data_.begin(), to), data);
        NotifyDataMoved(from, to);
    }

private:
    std::vector<std::shared_ptr<Int32>> data_;
};


class Delegate : public ListControlDelegate {
public:
    bool HasVariableItemHeight() override {
        return true;
    }

    float EstimateItemHeight(
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override {

        return float(As<Int32>(item_data)->Value() * 10);
    }

    std::wstring GetItemText(
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override {

        return item_data->ToString();
    }
};


bool CheckItem(const std::shared_ptr<ListItem>& item, std::int32_t value, float y) {

    if (item->Position().y != y) {
        return false;
    }

    auto int32_value = As<Int32>(item->ItemData());
    return int32_value->Value() == value;
}

}


TEST(ListControlMoveTest, MoveData) {

    auto data_source = Create<DataSource>();
    auto delegate = Create<Delegate>();

    auto list = Create<ListControl>();
    list->SetSize(Size{ 100, 1000 });
    list->SetDataSource(data_source);
    list->SetDelegate(delegate);

    {
        data_source->Move(1, 3);
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(0), 1, 0));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(1), 3, 10));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(2), 4, 40));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(3), 2, 80));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(4), 5, 100));
    }

    {
        data_source->Move(3, 1);
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(0), 1, 0));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(1), 2, 10));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(2), 3, 30));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(3), 4, 60));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(4), 5, 100));
    }

    {
        data_source->Move(0, 1);
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(0), 2, 0));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(1), 1, 20));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(2), 3, 30));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(3), 4, 60));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(4), 5, 100));
    }

    {
        data_source->Move(0, 4);
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(0), 1, 0));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(1), 3, 10));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(2), 4, 40));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(3), 5, 80));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(4), 2, 130));
    }

    {
        data_source->Move(4, 3);
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(0), 1, 0));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(1), 3, 10));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(2), 4, 40));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(3), 2, 80));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(4), 5, 100));
    }

    {
        data_source->Move(4, 0);
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(0), 5, 0));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(1), 1, 50));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(2), 3, 60));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(3), 4, 90));
        ASSERT_TRUE(CheckItem(list->GetVisibleItemAtIndex(4), 2, 130));
    }
}


TEST(ListControlMoveTest, Selection) {

    auto data_source = Create<DataSource>();
    auto delegate = Create<Delegate>();

    auto list = Create<ListControl>();
    list->SetSize(Size{ 100, 1000 });
    list->SetDataSource(data_source);
    list->SetDelegate(delegate);
    list->SelectItemAtIndex(1);

    bool selection_changed{};
    auto sub = list->SelectionChangedEvent().Subscribe(std::bind([&]() {
        selection_changed = true;
    }));

    {
        selection_changed = false;
        data_source->Move(0, 2);
        ASSERT_EQ(list->FirstSelectedItemIndex(), 0);
        ASSERT_TRUE(selection_changed);
    }

    {
        selection_changed = false;
        data_source->Move(0, 2);
        ASSERT_EQ(list->FirstSelectedItemIndex(), 2);
        ASSERT_TRUE(selection_changed);
    }
}