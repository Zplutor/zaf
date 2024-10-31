#include <zaf/control/property_grid/enum_value_view.h>
#include <zaf/base/container/utility/sort.h>
#include <zaf/base/container/utility/unique.h>
#include <zaf/base/range.h>
#include <zaf/object/enum_type.h>
#include <zaf/creation.h>

namespace zaf::property_grid {

void EnumValueView::Initialize() {

    __super::Initialize();

    combo_box_ = Create<ComboBox>();
    combo_box_->SetBorder(Frame{});
    combo_box_->SetTextInset(Frame{ 0, 1, 1, 1 });

    Subscriptions() += combo_box_->FocusGainedEvent().Subscribe(std::bind([this]() {
        NotifyShouldSelectItem();
    }));

    this->AddChild(combo_box_);
}


void EnumValueView::SetAccessMethod(AccessMethod access_method) {

    if (access_method == AccessMethod::ReadOnly) {
        combo_box_->SetIsEnabled(false);
    }
}


void EnumValueView::SetValue(const std::shared_ptr<Object>& value) {

    auto enum_type = dynamic_cast<EnumType*>(value->DynamicType());
    ZAF_EXPECT(enum_type);

    auto all_enum_values = enum_type->Values();
    InitializeComboBoxValues(all_enum_values, value);

    Subscriptions() += combo_box_->SelectionChangedEvent().Subscribe(
        std::bind(&EnumValueView::OnSelectionChanged, this));
}


void EnumValueView::InitializeComboBoxValues(
    const std::vector<std::shared_ptr<Object>>& values,
    const std::shared_ptr<Object>& selected_object) {

    auto drop_down_list = combo_box_->DropDownListBox();
    auto update_gurad = drop_down_list->BeginUpdate();

    for (auto index : Range(0, values.size())) {

        drop_down_list->AddItem(values[index]);

        if (selected_object->IsEqual(*values[index])) {
            drop_down_list->SelectItemAtIndex(index);
        }
    }
}


void EnumValueView::OnSelectionChanged() {

    NotifyValueChanged(combo_box_->DropDownListBox()->FirstSelectedItemData());
}

}