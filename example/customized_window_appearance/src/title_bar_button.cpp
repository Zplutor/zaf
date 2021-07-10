#include "title_bar_button.h"
#include <zaf/graphic/canvas.h>

TitleBarButton::TitleBarButton(Type type) : type_(type) {

}


void TitleBarButton::Initialize() {

    __super::Initialize();

    SetBackgroundColor(zaf::Color::Transparent());
}


void TitleBarButton::Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    auto content_rect = ContentRect();

    zaf::Ellipse ellipse;
    ellipse.x_radius = content_rect.size.width / 2;
    ellipse.y_radius = content_rect.size.height / 2;
    ellipse.position.x = content_rect.position.x + ellipse.x_radius;
    ellipse.position.y = content_rect.position.y + ellipse.y_radius;

    canvas.SetBrushWithColor(GetBackgroundColor());
    canvas.DrawEllipse(ellipse);

    canvas.SetBrushWithColor(GetBorderColor());
    canvas.DrawEllipseFrame(ellipse, 1);
}


zaf::Color TitleBarButton::GetBackgroundColor() const {

    switch (type_) {
    case Type::Close:
        return zaf::Color(1, float(0.38), float(0.35));
    case Type::Minimize:
        return zaf::Color(1, float(0.75), float(0.18));
    case Type::Maximize:
        return zaf::Color(float(0.47), float(0.8), float(0.26));
    default:
        return zaf::Color::Transparent();
    }
}


zaf::Color TitleBarButton::GetBorderColor() const {

    switch (type_) {
    case Type::Close:
        return zaf::Color(float(0.95), float(0.34), float(0.31));
    case Type::Minimize:
    case Type::Maximize:
    default:
        return zaf::Color::Transparent();
    }
}