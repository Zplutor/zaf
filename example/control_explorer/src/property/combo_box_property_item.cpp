#include "property/combo_box_property_item.h"
#include <zaf/control/combo_box.h>

std::shared_ptr<PropertyItem> CreateComboBoxPropertyItem(
    const std::wstring& title,
    const std::vector<std::wstring>& values,
    const std::function<std::wstring()>& get_value,
    const std::function<void(const std::wstring&)>& value_change,
    const std::function<void(const std::function<void()>&)>& register_notification) {

    auto combo_box = zaf::Create<zaf::ComboBox>();

    auto drop_down_list = combo_box->GetDropDownListBox();
    auto current_value = get_value();
    for (const auto& each_value : values) {
        auto index = drop_down_list->AddItemWithText(each_value);
        if (current_value == each_value) {
            drop_down_list->SelectItemAtIndex(index);
        }
    }

    auto update_value = [combo_box, get_value]() {
        auto value = get_value();
        auto drop_down_list = combo_box->GetDropDownListBox();
        for (std::size_t index = 0; index < drop_down_list->GetItemCount(); ++index) {
            if (value == drop_down_list->GetItemTextAtIndex(index)) {
                drop_down_list->SelectItemAtIndex(index);
            }
        }
    };

    combo_box->GetTextChangeEvent().AddListener([value_change, update_value](const std::shared_ptr<zaf::TextualControl>& control) {

        auto combo_box = dynamic_cast<zaf::ComboBox*>(control.get());

        auto value = combo_box->GetText();
        value_change(value);

        update_value();
    });

    if (register_notification != nullptr) {
        register_notification([update_value]() {
            update_value();
        });
    }

    return CreatePropertyItem(title, combo_box);
}