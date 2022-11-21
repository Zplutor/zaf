#include <zaf/control/property_grid/color_value_view.h>
#include <sstream>
#include <zaf/base/as.h>
#include <zaf/base/string/case_conversion.h>
#include <zaf/base/string/to_string.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>

namespace zaf::property_grid {

void ColorValueView::Initialize() {

    __super::Initialize();

    color_box_ = Create<Control>();
    color_box_->SetFixedWidth(28);
    color_box_->SetBorder(Frame{ 1 });
    color_box_->SetBorderColor(Color::Black());
    color_box_->SetMargin(Frame{ 0, 2, 4, 2 });

    color_label_ = Create<Label>();
    color_label_->SetParagraphAlignment(ParagraphAlignment::Center);

    this->AddChildren({ color_box_, color_label_ });
}


void ColorValueView::SetAccessMethod(AccessMethod access_method) {

    if (access_method == AccessMethod::ReadOnly) {
        color_label_->SetTextColor(Color::FromRGB(internal::ControlDisabledTextColorRGB));
    }
    else {
        color_label_->SetTextColor(Color::FromRGB(internal::ControlNormalTextColorRGB));
    }
}


void ColorValueView::SetValue(const std::shared_ptr<Object>& object) {

    if (!object) {
        color_box_->SetIsVisible(false);
        color_label_->SetIsVisible(false);
        return;
    }

    auto color = As<Color>(object);
    ZAF_EXPECT(color);

    color_box_->SetBackgroundColor(*color);
    color_box_->SetIsVisible(true);

    SetLabelText(*color);
}


void ColorValueView::SetLabelText(const Color& color) {

    auto float_to_hex = [](float value) {

        auto hex = zaf::ToWideString(
            static_cast<std::uint8_t>(value * 255),
            zaf::ToStringOptions().Base(16));

        if (hex.length() == 1) {
            hex = L'0' + hex;
        }

        return zaf::ToUppercased(hex);
    };

    std::wstring text(1, L'#');
    for (auto each_value : { color.a, color.r, color.g, color.b }) {
        text.append(float_to_hex(each_value));
    }

    color_label_->SetText(text);
    color_box_->SetIsVisible(true);
}

}