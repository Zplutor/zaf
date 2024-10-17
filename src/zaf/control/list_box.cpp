#include <zaf/control/list_box.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/control/control_object.h>
#include <zaf/control/list_box_delegate.h>
#include <zaf/control/list_box_data_source.h>
#include <zaf/creation.h>

namespace zaf {

ZAF_OBJECT_IMPL(ListBox);


ListBox::ListBox() {

}


ListBox::~ListBox() {

}


void ListBox::Initialize() {
    
    __super::Initialize();

    data_source_ = std::make_shared<ListBoxDataSource>();
    SetDataSource(data_source_);

    delegate_ = std::make_shared<ListBoxDelegate>();
    delegate_->SetItemHeight(default_item_height_);
    SetDelegate(delegate_);
}


void ListBox::OnDataSourceChanged(const std::shared_ptr<ListDataSource>& previous_data_source) {

    __super::OnDataSourceChanged(previous_data_source);

    auto new_data_source = DataSource();
    if (data_source_ == new_data_source) {
        return;
    }

    data_source_ = As<ListBoxDataSource>(new_data_source);
    ZAF_EXPECT(data_source_);
}


void ListBox::OnDelegateChanged(const std::shared_ptr<ListControlDelegate>& previous_delegate) {

    __super::OnDelegateChanged(previous_delegate);

    auto new_delegate = Delegate();
    if (delegate_ == new_delegate) {
        return;
    }

    delegate_ = As<ListBoxDelegate>(new_delegate);
    ZAF_EXPECT(delegate_);

    delegate_->SetItemHeight(default_item_height_);
}


void ListBox::SetDefaultItemHeight(float item_height) {

    default_item_height_ = item_height;

    delegate_->SetItemHeight(default_item_height_);
    Reload();
}


void ListBox::AddItem(const std::shared_ptr<Object>& data) {
    AddItemAtIndex(ItemCount(), data);
}


void ListBox::AddItemAtIndex(std::size_t index, const std::shared_ptr<Object>& data) {
    data_source_->AddDataAtIndex(index, data);
}


void ListBox::RemoveItemAtIndex(std::size_t index) {
    data_source_->RemoveDataAtIndex(index);
}


void ListBox::RemoveAllItems() {
    data_source_->RemoveAllData();
}

}
