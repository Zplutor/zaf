#include <gtest/gtest.h>
#include <zaf/control/list_control.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include <zaf/control/list_item_container.h>
#include <zaf/object/boxing/boxing.h>

/*
In this test case, we raise an item update event first. When handling this event, the item control 
would be removed and re-created. In item control's OnParentChanged() method, we raise another item 
update event, to simulate a reentering situation.

In such situation before, a duplidated item control would be created and added to list control. It 
shouldn't happend after solve.
*/

namespace {

class DataSource : public zaf::ListDataSource {
public:
    std::size_t GetDataCount() {
        return 1;
    }

    std::shared_ptr<zaf::Object> GetDataAtIndex(std::size_t index) {
        return zaf::Box(value_);
    }

    void AddValue() {
        value_++;
    }

    void NotifyUpdate() {
        NotifyDataUpdate(0, 1);
    }

private:
    int value_{};
};


class Item : public zaf::ListItem {
public:
    explicit Item(const std::shared_ptr<DataSource>& data_source) : data_source_(data_source) {

    }

    void OnParentChanged(const zaf::ParentChangedInfo& event_info) override {

        if (!this->Parent()) {
            data_source_->AddValue();
            data_source_->NotifyUpdate();
        }
    }

private:
    std::shared_ptr<DataSource> data_source_;
};


class Delegate : public zaf::ListControlDelegate {
public:
    explicit Delegate(const std::shared_ptr<DataSource>& data_source) : data_source_(data_source) {

    }

    float EstimateItemHeight(
        std::size_t item_index,
        const std::shared_ptr<zaf::Object>& item_data) override {

        return 10;
    }

    std::shared_ptr<zaf::ListItem> CreateItem(
        std::size_t item_index,
        const std::shared_ptr<zaf::Object>& item_data) override {

        return zaf::Create<Item>(data_source_);
    }

private:
    std::shared_ptr<DataSource> data_source_;
};

}


TEST(ListControlReenterTest, ReenterUpdate) {

    auto list_control = zaf::Create<zaf::ListControl>();
    list_control->SetSize(zaf::Size(100, 100));

    auto data_source = zaf::Create<DataSource>();
    auto delegate = zaf::Create<Delegate>(data_source);

    list_control->SetDataSource(data_source);
    list_control->SetDelegate(delegate);

    data_source->NotifyUpdate();

    auto item_container = list_control->ItemContainer();
    ASSERT_EQ(item_container->ChildCount(), 1);

    auto item = zaf::As<Item>(item_container->Children()[0]);
    //Expected value is 2 due to AddValue() is called twice.
    //One occurs when the first item control is removed;
    //One occurs when reentering is detected and list control reloads, the second item control is 
    //removed.
    ASSERT_EQ(zaf::Unbox<int>(*item->ItemData()), 2);
}