#include <zaf/control/property_grid/color_value_view.h>
#include <sstream>
#include <zaf/base/as.h>
#include <zaf/base/range.h>
#include <zaf/base/string/case_conversion.h>
#include <zaf/base/string/to_string.h>
#include <zaf/creation.h>
#include <zaf/graphic/canvas.h>
#include <zaf/internal/theme.h>
#include <zaf/window/message/keyboard_message.h>

namespace zaf::property_grid {

void ColorValueView::Initialize() {

    __super::Initialize();

    InitializeColorSquare();
    InitializeRichEdit();
}


void ColorValueView::InitializeColorSquare() {

    color_square_ = Create<ColorSquare>();
    color_square_->SetFixedWidth(28);
    color_square_->SetBorder(Frame{ 1 });
    color_square_->SetBorderColor(Color::Black());
    color_square_->SetMargin(Frame{ 0, 2, 4, 2 });
    this->AddChild(color_square_);
}


void ColorValueView::InitializeRichEdit() {

    color_edit_ = Create<RichEdit>();
    color_edit_->SetBorder(Frame{});
    color_edit_->SetBackgroundColor(Color::Transparent());
    color_edit_->SetParagraphAlignment(ParagraphAlignment::Center);
    color_edit_->SetAllowBeep(false);

    Subscriptions() += color_edit_->KeyDownEvent().Subscribe(
        [this](const KeyDownInfo& event_info) {
    
        if (event_info.Message().Key() == Key::Enter) {
            ChangeColorByEdit();
            event_info.MarkAsHandled();
        }
    });

    Subscriptions() += color_edit_->FocusGainedEvent().Subscribe(std::bind([this]() {
        NotifyShouldSelectItem();
    }));

    Subscriptions() += color_edit_->FocusLostEvent().Subscribe(std::bind([this]() {
        ChangeColorByEdit();
    }));

    this->AddChild(color_edit_);
}


void ColorValueView::ChangeColorByEdit() {

    auto text = color_edit_->Text();

    try {

        auto new_color = Create<Color>();
        Color::Type->GetParser()->ParseFromAttribute(text, *new_color);

        if (*new_color == color_square_->Color()) {
            return;
        }

        NotifyValueChanged(new_color);
    }
    catch (const Error&) {
        SetColorToEdit(color_square_->Color());
    }
}


void ColorValueView::SetAccessMethod(AccessMethod access_method) {

    bool is_readonly = access_method == AccessMethod::ReadOnly;
    color_edit_->SetIsEnabled(!is_readonly);
    color_edit_->SetTextColor(Color::FromRGB(
        is_readonly ? 
        internal::ControlDisabledTextColorRGB : 
        internal::ControlNormalTextColorRGB));
}


void ColorValueView::SetValue(const std::shared_ptr<Object>& object) {

    if (!object) {
        color_square_->SetIsVisible(false);
        color_edit_->SetIsVisible(false);
        return;
    }

    auto color = As<Color>(object);
    ZAF_EXPECT(color);

    color_square_->SetColor(*color);
    color_square_->SetIsVisible(true);

    SetColorToEdit(*color);
}


void ColorValueView::SetColorToEdit(const Color& color) {

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

    color_edit_->SetText(text);
    color_edit_->SetIsVisible(true);
}


void ColorValueView::RecoverFocus() {
    color_edit_->SetIsFocused(true);
}


void ColorValueView::ColorSquare::SetColor(const zaf::Color& color) {
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
    auto clipping_guard = canvas.PushClipping(content_rect);

    canvas.SetBrushWithColor(Color::White());
    canvas.DrawRectangle(content_rect);

    constexpr float square_size = 4.f;
    auto line_count = static_cast<int>(std::ceil(content_rect.size.height / square_size));
    auto column_count = static_cast<int>(std::ceil(content_rect.size.width / square_size));

    auto dim_square_color = Color::FromRGB(0xe5e5e5);
    canvas.SetBrushWithColor(dim_square_color);

    for (auto line : Range(0, line_count)) {
        for (auto column : Range(0, column_count)) {

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
}

}