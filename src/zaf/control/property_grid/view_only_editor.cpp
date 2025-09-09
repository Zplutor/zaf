#include <zaf/control/property_grid/view_only_editor.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>

namespace zaf::property_grid {

void ViewOnlyEditor::Initialize() {

    __super::Initialize();

    label_ = Create<Label>();
    label_->SetParagraphAlignment(dwrite::ParagraphAlignment::Center);
    label_->SetTextTrimming(zaf::dwrite::TextTrimmingGranularity::Character);

    this->AddChild(label_);
}


void ViewOnlyEditor::SetAccessMethod(AccessMethod access_method) {

    if (access_method == AccessMethod::ReadOnly) {
        label_->SetTextColor(Color::FromRGB(zaf::internal::ControlDisabledTextColorRGB));
    }
    else {
        label_->SetTextColor(Color::FromRGB(zaf::internal::ControlNormalTextColorRGB));
    }
}


void ViewOnlyEditor::SetValue(const std::shared_ptr<dynamic::Object>& value) {

    if (value) {
        label_->SetText(value->ToString());
    }
}

}