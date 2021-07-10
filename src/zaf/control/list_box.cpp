#include <zaf/control/list_box.h>
#include <zaf/base/error/check.h>
#include <zaf/control/list_box_delegate.h>
#include <zaf/control/list_box_data_source.h>
#include <zaf/creation.h>
#include <zaf/parsing/parsers/list_box_parser.h>
#include <zaf/object/type_definition.h>
#include <zaf/serialization/properties.h>

namespace zaf {

ZAF_DEFINE_TYPE(ListBox)
    ZAF_DEFINE_TYPE_PARSER(ListBoxParser)
ZAF_DEFINE_TYPE_END


ListBox::ListBox() {

}


ListBox::~ListBox() {

}


void ListBox::Initialize() {
    
    __super::Initialize();

    SetDataSource(std::make_shared<ListBoxDataSource>());
    SetDelegate(std::make_shared<ListBoxDelegate>());
}


void ListBox::DataSourceChange(const std::shared_ptr<ListDataSource>& previous_data_source) {

    __super::DataSourceChange(previous_data_source);

    data_source_ = dynamic_cast<ListBoxDataSource*>(GetDataSource().get());
    ZAF_EXPECT(data_source_);
}


void ListBox::DelegateChange(const std::shared_ptr<ListControlDelegate>& previous_delegate) {

    __super::DelegateChange(previous_delegate);

    delegate_ = dynamic_cast<ListBoxDelegate*>(GetDelegate().get());
    if (delegate_) {
        delegate_->SetItemHeight(default_item_height_);
    }
}


void ListBox::SetDefaultItemHeight(float item_height) {

    default_item_height_ = item_height;

    delegate_->SetItemHeight(default_item_height_);
    Reload();
}


void ListBox::AddItem(const std::shared_ptr<Object>& data) {
    AddItemAtIndex(GetItemCount(), data);
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
