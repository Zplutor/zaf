#include <zaf/control/property_grid/enum_editor.h>
#include <zaf/base/container/utility/sort.h>
#include <zaf/base/container/utility/unique.h>
#include <zaf/base/range.h>
#include <zaf/dynamic/enum_type.h>
#include <zaf/creation.h>

namespace zaf::property_grid {

void EnumEditor::Initialize() {

    __super::Initialize();

    combo_box_ = Create<ComboBox>();
    combo_box_->SetBorder(Frame{});
    combo_box_->SetTextInset(Frame{ 0, 1, 1, 1 });

    this->AddChild(combo_box_);
}


void EnumEditor::SetAccessMethod(AccessMethod access_method) {

    if (access_method == AccessMethod::ReadOnly) {
        combo_box_->SetIsEnabled(false);
    }
}


void EnumEditor::SetValue(const std::shared_ptr<dynamic::Object>& value) {

    auto enum_type = dynamic_cast<dynamic::EnumType*>(value->DynamicType());
    ZAF_EXPECT(enum_type);

    auto all_enum_values = enum_type->Values();
    InitializeComboBoxValues(all_enum_values, value);

    Disposables() += combo_box_->SelectionChangedEvent().Subscribe(
        std::bind(&EnumEditor::OnSelectionChanged, this));
}


void EnumEditor::InitializeComboBoxValues(
    const std::vector<std::shared_ptr<dynamic::Object>>& values,
    const std::shared_ptr<dynamic::Object>& selected_object) {

    auto drop_down_list = combo_box_->DropDownListBox();
    auto update_gurad = drop_down_list->BeginUpdate();

    for (auto index : Range(0, values.size())) {

        drop_down_list->AddItem(values[index]);

        if (selected_object->IsEqual(*values[index])) {
            drop_down_list->SelectItemAtIndex(index);
        }
    }
}


void EnumEditor::OnSelectionChanged() {

    NotifyValueChanged(combo_box_->DropDownListBox()->FirstSelectedItemData());
}

}