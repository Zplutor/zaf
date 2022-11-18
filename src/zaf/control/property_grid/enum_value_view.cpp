#include <zaf/control/property_grid/enum_value_view.h>
#include <zaf/base/container/utility/range.h>
#include <zaf/base/container/utility/sort.h>
#include <zaf/base/container/utility/unique.h>
#include <zaf/object/enum_type.h>
#include <zaf/creation.h>

namespace zaf::property_grid {

void EnumValueView::Initialize() {

    __super::Initialize();

    combo_box_ = Create<ComboBox>();
    combo_box_->SetBorder(Frame{});
    this->AddChild(combo_box_);
}


void EnumValueView::SetAccessMethod(AccessMethod) {

}


void EnumValueView::SetValue(const std::shared_ptr<Object>& object) {

    if (!object) {
        return;
    }

    auto enum_type = dynamic_cast<EnumType*>(object->GetType());
    ZAF_EXPECT(enum_type);

    auto all_enum_values = enum_type->GetAllValues();
    InitializeComboBoxValues(all_enum_values, object);
}


void EnumValueView::InitializeComboBoxValues(
    const std::vector<std::shared_ptr<Object>>& values,
    const std::shared_ptr<Object>& selected_object) {

    auto drop_down_list = combo_box_->GetDropDownListBox();
    auto update_gurad = drop_down_list->BeginUpdate();

    for (auto index : zaf::Range(0, values.size())) {

        drop_down_list->AddItem(values[index]);

        if (selected_object->IsEqual(*values[index])) {
            drop_down_list->SelectItemAtIndex(index);
        }
    }
}

}