#include <functional>
#include <gtest/gtest.h>
#include <zaf/control/linear_box.h>
#include <zaf/control/list_control.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include "utility/test_window.h"

using namespace zaf;

namespace {

class TestDataSource : public ListDataSource {
public:
    TestDataSource() {
        for (auto index : Range(0, 10)) {
            data_list_.push_back(Box(static_cast<int>(index)));
        }
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


class TestDelegate : public ListControlDelegate {
public:
    float EstimateItemHeight(
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override {

        return 10;
    }
};


void TestWithListControl(const std::function<void(Window&, ListControl&)> test) {

    TestWithWindow([&test](Window& window) {
    
        auto list_control = Create<ListControl>();
        list_control->SetBorder({});
        list_control->SetPadding({});
        list_control->SetFixedHeight(200);

        auto data_source = Create<TestDataSource>();
        list_control->SetDataSource(data_source);

        auto delegate = Create<TestDelegate>();
        list_control->SetDelegate(delegate);

        auto vertical_box = Create<VerticalBox>();
        vertical_box->AddChild(list_control);

        window.SetRootControl(vertical_box);

        test(window, *list_control);
    });
}

}


TEST(ListControlTest, MouseDown_NoneSelectionMode) {

    TestWithListControl([](Window& window, ListControl& list_control) {

        list_control.SetSelectionMode(SelectionMode::None);
        window.Messager().SendWMLBUTTONDOWN(Point{10, 10});
        ASSERT_EQ(list_control.SelectedItemCount(), 0);
    });
}


TEST(ListControlTest, MouseDown_SingleSelectionMode) {

    TestWithListControl([](Window& window, ListControl& list_control) {

        list_control.SetSelectionMode(SelectionMode::Single);

        std::size_t selection_changed_count{};
        auto sub = list_control.SelectionChangedEvent().Subscribe(
            std::bind([&selection_changed_count]() {
                selection_changed_count++;
            }));
        
        window.Messager().SendWMLBUTTONDOWN(Point{ 10, 15 });
        ASSERT_EQ(list_control.SelectedItemCount(), 1);
        ASSERT_EQ(list_control.FirstSelectedItemIndex(), 1);
        ASSERT_EQ(list_control.GetVisibleItemAtIndex(1)->IsSelected(), true);
        ASSERT_EQ(selection_changed_count, 0);

        window.Messager().SendWMMOUSEMOVE(Point{ 10, 25 });
        ASSERT_EQ(list_control.SelectedItemCount(), 1);
        ASSERT_EQ(list_control.FirstSelectedItemIndex(), 2);
        ASSERT_EQ(list_control.GetVisibleItemAtIndex(1)->IsSelected(), false);
        ASSERT_EQ(list_control.GetVisibleItemAtIndex(2)->IsSelected(), true);
        ASSERT_EQ(selection_changed_count, 0);

        window.Messager().SendWMLBUTTONUP(Point{ 10, 25 });
        ASSERT_EQ(list_control.SelectedItemCount(), 1);
        ASSERT_EQ(list_control.FirstSelectedItemIndex(), 2);
        ASSERT_EQ(list_control.GetVisibleItemAtIndex(1)->IsSelected(), false);
        ASSERT_EQ(list_control.GetVisibleItemAtIndex(2)->IsSelected(), true);
        ASSERT_EQ(selection_changed_count, 1);
    });
}