#include <zaf/control/list_box_delegate.h>
#include <zaf/control/label_list_item.h>

namespace zaf {

float ListBoxDelegate::EstimateItemHeight(
    std::size_t item_index, 
    const std::shared_ptr<Object>& item_data) {

    return item_height_;
}


std::shared_ptr<ListItem> ListBoxDelegate::CreateItem(
    std::size_t item_index, 
    const std::shared_ptr<Object>& item_data) {

    auto list_item = zaf::Create<LabelListItem>();
    if (text_color_picker_) {
        list_item->Label()->SetTextColorPicker(text_color_picker_);
    }
    if (background_color_picker_) {
        list_item->SetBackgroundColorPicker(background_color_picker_);
    }
    return list_item;
}

}