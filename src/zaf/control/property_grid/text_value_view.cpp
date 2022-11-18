#include <zaf/control/property_grid/text_value_view.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>

namespace zaf::property_grid {

void TextValueView::Initialize() {

    __super::Initialize();

    label_ = Create<Label>();
    label_->SetParagraphAlignment(ParagraphAlignment::Center);
    label_->SetTextTrimming(zaf::TextTrimming::Granularity::Character);

    this->AddChild(label_);
}


void TextValueView::SetAccessMethod(AccessMethod) {

}


void TextValueView::SetValue(const std::shared_ptr<Object>& value) {

    if (value) {
        label_->SetText(value->ToString());
    }
}

}