#include <zaf/control/scrollable_control.h>
#include <zaf/control/internal/scrollable_control/general_layouter.h>
#include <zaf/control/internal/scrollable_control/self_scrolling_layouter.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/self_scrolling_control.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>
#include <zaf/parsing/parsers/scrollable_control_parser.h>
#include <zaf/reflection/reflection_type_definition.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {
namespace {

std::shared_ptr<Control> CreateDefaultScrollContentControl() {
    auto control = Create<Control>();
    control->SetBackgroundColor(Color::Transparent);
    return control;
}

const wchar_t* const kAllowHorizontalScrollPropertyName = L"AllowHorizontalScroll";
const wchar_t* const kAllowVerticalScrollPropertyName = L"AllowVerticalScroll";
const wchar_t* const kAutoHideScrollBarsPropertyName = L"AutoHideScrollBars";
const wchar_t* const kScrollBarThicknessPropertyName = L"ScrollBarThickness";

}


ZAF_DEFINE_REFLECTION_TYPE(ScrollableControl);


ScrollableControl::ScrollableControl() :
    self_scrolling_control_(nullptr) {

}


ScrollableControl::~ScrollableControl() {

    //Layouter must be the first member to destroy for unregistering 
    //events from other members.
    layouter_.reset();
}


void ScrollableControl::Initialize() {

    SetBorder(1);

    SetBorderColorPicker([](const Control&) {
        return Color::Black;
    });

    InitializeVerticalScrollBar(Create<ScrollBar>());
    InitializeHorizontalScrollBar(Create<ScrollBar>());
    InitializeScrollBarCorner(Create<Control>());

    scroll_container_control_ = Create<Control>();
    scroll_container_control_->SetBackgroundColor(Color::Transparent);
    AddChild(scroll_container_control_);

    InitializeScrollContentControl(CreateDefaultScrollContentControl());
    InitializeLayouter();
}


void ScrollableControl::InitializeScrollContentControl(const std::shared_ptr<Control>& control) {

    scroll_content_control_ = control;
    scroll_content_control_->SetIsCachedPaintingEnabled(true);
    scroll_container_control_->AddChild(scroll_content_control_);

    self_scrolling_control_ = dynamic_cast<SelfScrollingControl*>(control.get());
    if (self_scrolling_control_ != nullptr) {

        self_scrolling_control_->SetAllowVerticalScroll(AllowVerticalScroll());
        self_scrolling_control_->SetAllowHorizontalScroll(AllowHorizontalScroll());
        self_scrolling_control_->SetAutoHideScrollBars(AutoHideScrollBars());   
    }
}


void ScrollableControl::InitializeVerticalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    vertical_scroll_bar_ = scroll_bar;
    AddChild(vertical_scroll_bar_);
}


void ScrollableControl::InitializeHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    horizontal_scroll_bar_ = scroll_bar;
    horizontal_scroll_bar_->SetIsHorizontal(true);
    AddChild(horizontal_scroll_bar_);
}


void ScrollableControl::InitializeScrollBarCorner(const std::shared_ptr<Control>& corner) {

    scroll_bar_corner_ = corner;
    AddChild(scroll_bar_corner_);
}


void ScrollableControl::InitializeLayouter() {

    if (self_scrolling_control_ == nullptr) {
        layouter_ = std::make_unique<internal::GeneralLayouter>(this);
    }
    else {
        layouter_ = std::make_unique<internal::SelfScrollingLayouter>(this);
    }
}


void ScrollableControl::Layout(const Rect& previous_rect) {
    if (layouter_ != nullptr) {
        layouter_->Layout();
    }
}


bool ScrollableControl::AllowVerticalScroll() const {

    auto allow_scroll = GetPropertyMap().TryGetProperty<bool>(kAllowVerticalScrollPropertyName);
    if (allow_scroll != nullptr) {
        return *allow_scroll;
    }
    else {
        return true;
    }
}


void ScrollableControl::SetAllowVerticalScroll(bool allow_scroll) {

    GetPropertyMap().SetProperty(kAllowVerticalScrollPropertyName, allow_scroll);

    if (self_scrolling_control_ != nullptr) {
        self_scrolling_control_->SetAllowVerticalScroll(allow_scroll);
    }

    NeedRelayout();
}


bool ScrollableControl::AllowHorizontalScroll() const {

    auto allow_scroll = GetPropertyMap().TryGetProperty<bool>(kAllowHorizontalScrollPropertyName);
    if (allow_scroll != nullptr) {
        return *allow_scroll;
    }
    else {
        return true;
    }
}


void ScrollableControl::SetAllowHorizontalScroll(bool allow_scroll) {

    GetPropertyMap().SetProperty(kAllowHorizontalScrollPropertyName, allow_scroll);

    if (self_scrolling_control_ != nullptr) {
        self_scrolling_control_->SetAllowHorizontalScroll(allow_scroll);
    }

    NeedRelayout();
}


bool ScrollableControl::AutoHideScrollBars() const {

    auto always_show = GetPropertyMap().TryGetProperty<bool>(kAutoHideScrollBarsPropertyName);
    if (always_show != nullptr) {
        return *always_show;
    }
    else {
        return false;
    }
}


void ScrollableControl::SetAutoHideScrollBars(bool auto_hide) {

    GetPropertyMap().SetProperty(kAutoHideScrollBarsPropertyName, auto_hide);

    if (self_scrolling_control_ != nullptr) {
        self_scrolling_control_->SetAutoHideScrollBars(auto_hide);
    }

    NeedRelayout();
}


void ScrollableControl::SetVerticalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    auto previous_scroll_bar = vertical_scroll_bar_;
    if (previous_scroll_bar == scroll_bar) {
        return;
    }

    UpdateGuard update_guard(*this);

    RemoveChild(previous_scroll_bar);
    InitializeVerticalScrollBar(scroll_bar != nullptr ? scroll_bar : Create<ScrollBar>());

    layouter_->ScrollBarChange(false, previous_scroll_bar);
    VerticalScrollBarChange(previous_scroll_bar);
    NeedRelayout();
}


void ScrollableControl::SetHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    auto previous_scroll_bar = horizontal_scroll_bar_;
    if (previous_scroll_bar == scroll_bar) {
        return;
    }

    UpdateGuard update_guard(*this);

    RemoveChild(previous_scroll_bar);
    InitializeHorizontalScrollBar(scroll_bar != nullptr ? scroll_bar : Create<ScrollBar>());

    layouter_->ScrollBarChange(true, previous_scroll_bar);
    HorizontalScrollBarChange(previous_scroll_bar);
    NeedRelayout();
}


void ScrollableControl::SetScrollBarCorner(const std::shared_ptr<Control>& control) {

    auto previous_corner = scroll_bar_corner_;
    if (previous_corner == control) {
        return;
    }

    UpdateGuard update_guard(*this);

    RemoveChild(previous_corner);
    InitializeScrollBarCorner(control != nullptr ? control : Create<Control>());

    ScrollBarCornerChange(previous_corner);
    NeedRelayout();
}


void ScrollableControl::SetScrollContentControl(const std::shared_ptr<Control>& control) {

    auto previous_control = scroll_content_control_;
    if (previous_control == control) {
        return;
    }

    UpdateGuard update_guard(*this);

    scroll_container_control_->RemoveChild(previous_control);

    //Destroy layouter first for unregistering events before changing scroll content control.
    layouter_.reset();
    InitializeScrollContentControl(control != nullptr ? control : CreateDefaultScrollContentControl());
    InitializeLayouter();

    ScrollContentControlChange(previous_control);
    NeedRelayout();
}


float ScrollableControl::GetScrollBarThickness() const {

    auto thickness = GetPropertyMap().TryGetProperty<float>(kScrollBarThicknessPropertyName);
    if (thickness != nullptr) {
        return *thickness;
    }
    else {
        return 17;
    }
}


void ScrollableControl::SetScrollBarThickness(float thickness) {

    GetPropertyMap().SetProperty(kScrollBarThicknessPropertyName, thickness);
    NeedRelayout();
}


void ScrollableControl::SetScrollContentSize(const Size& size) {

    if ((expected_scroll_content_size_.width == size.width) &&
        (expected_scroll_content_size_.height == size.height)) {
        return;
    }

    expected_scroll_content_size_ = size;
    NeedRelayout();
}


bool ScrollableControl::MouseWheel(const Point& position, const MouseWheelMessage& message) {

    std::shared_ptr<ScrollBar> scroll_bar;

    if (message.IsHorizontalWheeling()) {
        scroll_bar = horizontal_scroll_bar_;   
    }
    else {
        scroll_bar = vertical_scroll_bar_;
    }

    if (scroll_bar->IsVisible() && scroll_bar->IsEnabled()) {
        scroll_bar->Wheel(message.GetWheelingDistance());
    }

    return true;
}


void ScrollableControl::IsEnabledChange() {

    __super::IsEnabledChange();

    if (IsEnabled()) { 
        //Relayout to refresh scroll bars.
        NeedRelayout();
    }
}


Rect ScrollableControl::GetVisibleScrollContentRect() const {

    if (self_scrolling_control_ != nullptr) {
        return scroll_container_control_->GetRect();
    }

    Rect rect;
    rect.position = scroll_content_control_->GetPosition();
    rect.position.x *= -1;
    rect.position.y *= -1;
    rect.size = scroll_container_control_->GetSize();
    return rect;
}


void ScrollableControl::ScrollToScrollContentPosition(const Point& position) {

    if (self_scrolling_control_ != nullptr) {
        return;
    }

    if (AllowHorizontalScroll() && horizontal_scroll_bar_->IsEnabled()) {
        horizontal_scroll_bar_->SetValue(static_cast<int>(position.x));
    }

    if (AllowVerticalScroll() && vertical_scroll_bar_->IsEnabled()) {
        vertical_scroll_bar_->SetValue(static_cast<int>(position.y));
    }
}


void ScrollableControl::ScrollUpToBegin() {
    const auto& vertical_scroll_bar = GetVerticalScrollBar();
    vertical_scroll_bar->SetValue(vertical_scroll_bar->GetMinimumValue()); 
}


void ScrollableControl::ScrollDownToEnd() {
    const auto& vertical_scroll_bar = GetVerticalScrollBar();
    vertical_scroll_bar->SetValue(vertical_scroll_bar->GetMaximumValue());
}


void ScrollableControl::ScrollLeftToBegin() {
    const auto& horizontal_scroll_bar = GetHorizontalScrollBar();
    horizontal_scroll_bar->SetValue(horizontal_scroll_bar->GetMinimumValue());
}


void ScrollableControl::ScrollRightToEnd() {
    const auto& horizontal_scroll_bar = GetHorizontalScrollBar();
    horizontal_scroll_bar->SetValue(horizontal_scroll_bar->GetMaximumValue());
}


}