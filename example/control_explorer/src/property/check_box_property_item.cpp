#include "property/check_box_property_item.h"
#include <zaf/base/as.h>
#include <zaf/control/check_box.h>
#include <zaf/creation.h>

std::shared_ptr<PropertyItem> CreateCheckBoxPropertyItem(
    const std::wstring& title,
    const std::function<bool()>& get_value,
    const std::function<void(bool)>& value_change,
    const std::function<void(const std::function<void()>&)>& register_notification) {

    auto check_box = zaf::Create<zaf::CheckBox>();
    check_box->SetParagraphAlignment(zaf::ParagraphAlignment::Center);
    check_box->SetIsChecked(get_value());

    check_box->Subscriptions() += check_box->CheckStateChangedEvent().Subscribe(
        [get_value, value_change](const zaf::CheckStateChangedInfo& event_info) {
            auto check_box = zaf::As<zaf::CheckBox>(event_info.Source());
            value_change(check_box->IsChecked());
            check_box->SetIsChecked(get_value());
        }
    );

    if (register_notification != nullptr) {
        register_notification([check_box, get_value]() {
            check_box->SetIsChecked(get_value());
        });
    }

    return CreatePropertyItem(title, check_box);
}