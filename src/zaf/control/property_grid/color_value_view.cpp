#include <zaf/control/property_grid/color_value_view.h>
#include <sstream>
#include <zaf/base/as.h>
#include <zaf/base/container/utility/range.h>
#include <zaf/base/string/case_conversion.h>
#include <zaf/base/string/to_string.h>
#include <zaf/creation.h>
#include <zaf/graphic/canvas.h>
#include <zaf/internal/theme.h>

namespace zaf::property_grid {

void ColorValueView::Initialize() {

    __super::Initialize();

    color_square_ = Create<ColorSquare>();
    color_square_->SetFixedWidth(28);
    color_square_->SetBorder(Frame{ 1 });
    color_square_->SetBorderColor(Color::Black());
    color_square_->SetMargin(Frame{ 0, 2, 4, 2 });

    color_label_ = Create<Label>();
    color_label_->SetParagraphAlignment(ParagraphAlignment::Center);

    this->AddChildren({ color_square_, color_label_ });
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
        color_square_->SetIsVisible(false);
        color_label_->SetIsVisible(false);
        return;
    }

    auto color = As<Color>(object);
    ZAF_EXPECT(color);

    color_square_->SetColor(*color);
    color_square_->SetIsVisible(true);

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
    color_square_->SetIsVisible(true);
}


void ColorValueView::ColorSquare::SetColor(const Color& color) {
    color_ = color;
    NeedRepaint();
}


void ColorValueView::ColorSquare::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    PaintTransparentColorSmallSquares(canvas);

    canvas.SetBrushWithColor(color_);
    canvas.DrawRectangle(ContentRect());
}


void ColorValueView::ColorSquare::PaintTransparentColorSmallSquares(Canvas& canvas) {

    auto content_rect = ContentRect();
    canvas.PushClippingRect(content_rect);

    canvas.SetBrushWithColor(Color::White());
    canvas.DrawRectangle(content_rect);

    constexpr float square_size = 4.f;
    auto line_count = static_cast<int>(std::ceil(content_rect.size.height / square_size));
    auto column_count = static_cast<int>(std::ceil(content_rect.size.width / square_size));

    auto dim_square_color = Color::FromRGB(0xe5e5e5);
    canvas.SetBrushWithColor(dim_square_color);

    for (auto line : zaf::Range(0, line_count)) {
        for (auto column : zaf::Range(0, column_count)) {

            bool should_paint = ((column + line % 2) % 2 == 0);
            if (!should_paint) {
                continue;
            }

            zaf::Rect square_rect;
            square_rect.position.x = content_rect.position.x + column * square_size;
            square_rect.position.y = content_rect.position.y + line * square_size;
            square_rect.size.width = square_size;
            square_rect.size.height = square_size;
            canvas.DrawRectangle(square_rect);
        }
    }

    canvas.PopClippingRect();
}

}