#include "operate/operate_item_creation.h"
#include <zaf/base/string/to_numeric.h>
#include <zaf/base/string/to_string.h>
#include <zaf/control/check_box.h>
#include <zaf/control/combo_box.h>
#include <zaf/creation.h>
#include "operate/value_text_box.h"

static std::shared_ptr<OperateItem> CreateOperateItem(
    const std::wstring& title, 
    const std::shared_ptr<zaf::Control>& value_control) {

    auto item = zaf::Create<OperateItem>();
    item->SetTitle(title);
    item->SetValueControl(value_control);
    return item;
}

static std::shared_ptr<ValueTextBox> CreateTextBox(
    const std::function<std::wstring()>& get_value,
    const std::function<void(const std::wstring&)>& value_change) {

    auto text_box = zaf::Create<ValueTextBox>();
    text_box->SetText(get_value());
    text_box->RegisterValueChangeEvent([get_value, value_change](ValueTextBox& text_box) {
   
        value_change(text_box.GetText());
        text_box.SetText(get_value());
    });
    return text_box;
}


std::shared_ptr<OperateItem> CreateOperateItemWithTextBox(
    const std::wstring& name,
    const std::function<std::wstring()>& get_value,
    const std::function<void(const std::wstring&)>& value_change,
    bool support_multiline) {

    auto text_box = CreateTextBox(get_value, value_change);
    text_box->SetIsMultiline(support_multiline);

    return CreateOperateItem(name, text_box);
}


template<typename ValueType>
std::shared_ptr<OperateItem> CreateOperateItemWithNumericTextBox(
    const std::wstring& name,
    const std::function<ValueType()>& get_value,
    const std::function<void(ValueType)>& value_change) {

    auto text_box = CreateTextBox(
        [get_value]() { 
            return zaf::ToWideString(get_value(), zaf::ToStringOptions().Precision(2)); 
        },
        [value_change](const std::wstring& text) {
            ValueType value = 0;
            if (zaf::TryToNumeric<ValueType>(text, value)) {
                value_change(value);
            }
        }
    );

    return CreateOperateItem(name, text_box);
}

std::shared_ptr<OperateItem> CreateOperateItemWithFloatTextBox(
    const std::wstring& name,
    const std::function<float()>& get_value,
    const std::function<void(float)>& value_change) {
    return CreateOperateItemWithNumericTextBox<float>(name, get_value, value_change);
}

std::shared_ptr<OperateItem> CreateOperateItemWithIntegerTextBox(
    const std::wstring& name,
    const std::function<std::int64_t()>& get_value,
    const std::function<void(std::int64_t)>& value_change) {
    return CreateOperateItemWithNumericTextBox<std::int64_t>(name, get_value, value_change);
}

std::shared_ptr<OperateItem> CreateOperateItemWithCheckBox(
    const std::wstring& name,
    const std::function<bool()>& get_value,
    const std::function<void(bool)>& value_change,
    const std::function<void(const std::function<void()>&)>& register_notification) {

    auto check_box = zaf::Create<zaf::CheckBox>();
    check_box->SetParagraphAlignment(zaf::ParagraphAlignment::Center);
    check_box->SetIsChecked(get_value());
    check_box->GetCheckStateChangeEvent().AddListener([get_value, value_change](const std::shared_ptr<zaf::CheckBox>& check_box) {
        value_change(check_box->IsChecked());
        check_box->SetIsChecked(get_value());
    });

    if (register_notification != nullptr) {
        register_notification([check_box, get_value]() {
            check_box->SetIsChecked(get_value());
        });
    }

    return CreateOperateItem(name, check_box);
}


std::shared_ptr<OperateItem> CreateOperateItemWithComboBox(
    const std::wstring& name, 
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

    return CreateOperateItem(name, combo_box);
}