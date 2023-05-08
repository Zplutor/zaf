#include <zaf/control/property_grid/text_value_view.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>

namespace zaf::property_grid {

void TextValueView::Initialize() {

    __super::Initialize();

    label_ = Create<Label>();
    label_->SetParagraphAlignment(ParagraphAlignment::Center);
    label_->SetTextTrimming(zaf::TextTrimmingGranularity::Character);

    this->AddChild(label_);
}


void TextValueView::SetAccessMethod(AccessMethod access_method) {

    if (access_method == AccessMethod::ReadOnly) {
        label_->SetTextColor(Color::FromRGB(zaf::internal::ControlDisabledTextColorRGB));
    }
    else {
        label_->SetTextColor(Color::FromRGB(zaf::internal::ControlNormalTextColorRGB));
    }
}


void TextValueView::SetValue(const std::shared_ptr<Object>& value) {

    if (value) {
        label_->SetText(value->ToString());
    }
}

}