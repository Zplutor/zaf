#include <zaf/control/menu_item.h>
#include <zaf/base/as.h>
#include <zaf/base/event_utility.h>
#include <zaf/control/internal/triangle_geometry.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/dpi.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/internal/theme.h>
#include <zaf/object/type_definition.h>
#include <zaf/window/popup_menu.h>

namespace zaf {
namespace {

constexpr float SubMenuArrowWidth = 8.f;
constexpr float SubMenuArrowMargin = 4.f;

constexpr const wchar_t* SubMenuShowEventPropertyName = L"SubMenuShowEvent";
constexpr const wchar_t* SubMenuCloseEventPropertyName = L"SubMenuCloseEvent";

}

ZAF_DEFINE_TYPE(MenuItem)
ZAF_DEFINE_TYPE_END;

void MenuItem::Initialize() {

    __super::Initialize();

    SetFixedHeight(30);
    SetPadding(Frame{ 8, 8, 8, 8 });
    SetTextAlignment(TextAlignment::Left);
    SetParagraphAlignment(ParagraphAlignment::Center);
    SetTextTrimming(TextTrimming::Granularity::Word);
}


void MenuItem::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    PaintSubMenuArrow(canvas);
}


void MenuItem::PaintSubMenuArrow(Canvas& canvas) {

    auto sub_menu_arrow_rect = GetSubMenuArrowRect();
    if (sub_menu_arrow_rect.IsEmpty()) {
        return;
    }

    auto triangle_geometry = internal::CreateTriangleGeometry(
        sub_menu_arrow_rect.size.width,
        sub_menu_arrow_rect.size.height,
        SubMenuArrowWidth,
        90);

    Point transform_position = ContentRect().position;
    transform_position.AddOffset(sub_menu_arrow_rect.position);

    auto transformed_geometry = GraphicFactory::Instance().CreateTransformedGeometry(
        triangle_geometry,
        TransformMatrix::Translation(transform_position));

    Canvas::StateGuard guard{ canvas };
    canvas.SetBrushWithColor(this->TextColor());
    canvas.DrawGeometry(transformed_geometry);
}


zaf::Rect MenuItem::GetTextRect() {

    auto result = __super::GetTextRect();
    if (HasSubMenuItem()) {
        result.Subtract(GetSubMenuArrowRect());
        result.size.width -= SubMenuArrowMargin;
    }
    return result;
}


zaf::Rect MenuItem::GetSubMenuArrowRect() {

    if (!HasSubMenuItem()) {
        return {};
    }

    zaf::Rect result{ zaf::Point{}, ContentSize() };
    result.position.x = result.Right() - SubMenuArrowWidth;
    result.size.width = SubMenuArrowWidth;
    return result;
}


zaf::Size MenuItem::CalculatePreferredContentSize(const zaf::Size& bound_size) const {

    auto result = __super::CalculatePreferredContentSize(bound_size);

    if (HasSubMenuItem()) {
        result.width += SubMenuArrowMargin + SubMenuArrowWidth;
    }

    return result;
}


void MenuItem::OnIsSelectedChanged() {

    __super::OnIsSelectedChanged();

    SetTextColor(IsSelected() ? zaf::Color::White() : zaf::Color::Black());
    SetBackgroundColor(
        IsSelected() ?
        zaf::Color::FromRGB(internal::ControlSelectedActivedColorRGB) : 
        zaf::Color::White());
}


void MenuItem::AddSubMenuItem(const std::shared_ptr<MenuItem>& sub_menu_item) {

    CheckCreateSubMenu();

    bool need_repaint = !HasSubMenuItem();

    sub_menu_->AddMenuItem(sub_menu_item);

    if (need_repaint) {
        NeedRepaint();
    }
}


void MenuItem::CheckCreateSubMenu() {

    if (sub_menu_) {
        return;
    }

    sub_menu_ = Create<PopupMenu>();

    Subscriptions() += sub_menu_->ShowEvent().Subscribe([this](const ShowInfo& event_info) {
    
        auto observer = GetEventObserver<SubMenuShowInfo>(
            GetPropertyMap(),
            SubMenuShowEventPropertyName);

        if (observer) {
            observer->OnNext(SubMenuShowInfo{ As<MenuItem>(shared_from_this()) });
        }
    });

    Subscriptions() += sub_menu_->DestroyedEvent().Subscribe(
        [this](const DestroyedInfo& event_info) {
    
        auto observer = GetEventObserver<SubMenuCloseInfo>(
            GetPropertyMap(),
            SubMenuCloseEventPropertyName);

        if (observer) {
            observer->OnNext(SubMenuCloseInfo{ As<MenuItem>(shared_from_this()) });
        }
    });
}


void MenuItem::RemoveSubMenuItem(const std::shared_ptr<MenuItem>& sub_menu_item) {

    if (!sub_menu_) {
        return;
    }

    sub_menu_->RemoveMenuItem(sub_menu_item);

    if (!sub_menu_->HasMenuItem()) {
        NeedRepaint();
    }
}


bool MenuItem::HasSubMenuItem() const {

    if (sub_menu_) {
        return sub_menu_->HasMenuItem();
    }
    return false;
}


void MenuItem::PopupSubMenu() {

    if (!sub_menu_) {
        return;
    }

    auto owning_menu = As<PopupMenu>(Window());
    if (!owning_menu) {
        return;
    }

    auto absolute_rect = this->AbsoluteRect();
    Point popup_position;
    popup_position.x = absolute_rect.Right();
    popup_position.y = absolute_rect.Top();

    sub_menu_->Popup(owning_menu, popup_position);
}


Observable<SubMenuShowInfo> MenuItem::SubMenuShowEvent() {
    return GetEventObservable<SubMenuShowInfo>(GetPropertyMap(), SubMenuShowEventPropertyName);
}


void MenuItem::CloseSubMenu() {

    if (sub_menu_) {
        sub_menu_->Close();
    }
}


Observable<SubMenuCloseInfo> MenuItem::SubMenuCloseEvent() {
    return GetEventObservable<SubMenuCloseInfo>(GetPropertyMap(), SubMenuCloseEventPropertyName);
}

}