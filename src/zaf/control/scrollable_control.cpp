#include <zaf/control/scrollable_control.h>
#include <zaf/control/internal/scrollable_control/general_layouter.h>
#include <zaf/control/internal/scrollable_control/self_scrolling_layouter.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/self_scrolling_control.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {
namespace {

const wchar_t* const kAllowHorizontalScrollPropertyName = L"AllowHorizontalScroll";
const wchar_t* const kAllowVerticalScrollPropertyName = L"AllowVerticalScroll";
const wchar_t* const kAutoHideScrollBarsPropertyName = L"AutoHideScrollBars";
const wchar_t* const kScrollBarThicknessPropertyName = L"ScrollBarThickness";

}

ScrollableControl::ScrollableControl() :
    self_scrolling_control_(nullptr) {

}


ScrollableControl::~ScrollableControl() {

}


void ScrollableControl::Initialize() {

    SetBorder(1);

    SetBorderColorPicker([](const Control&) {
        return Color::Black;
    });

    InitializeVerticalScrollBar(Create<ScrollBar>());
    InitializeHorizontalScrollBar(Create<ScrollBar>());

    scroll_container_control_ = Create<Control>();
    scroll_container_control_->SetBackgroundColor(Color::Transparent);
    AddChild(scroll_container_control_);

    auto scroll_content_control = Create<Control>();
    scroll_content_control->SetBackgroundColor(Color::Transparent);
    InitializeScrollContentControl(scroll_content_control);
    InitializeLayouter();
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


void ScrollableControl::InitializeScrollContentControl(const std::shared_ptr<Control>& control) {

    scroll_content_control_ = control;
    scroll_container_control_->AddChild(scroll_content_control_);

    self_scrolling_control_ = dynamic_cast<SelfScrollingControl*>(control.get());
    if (self_scrolling_control_ != nullptr) {

        self_scrolling_control_->SetAllowVerticalScroll(AllowVerticalScroll());
        self_scrolling_control_->SetAllowHorizontalScroll(AllowHorizontalScroll());
        self_scrolling_control_->SetAutoHideScrollBars(AutoHideScrollBars());
    }
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

    layouter_.reset();
    InitializeVerticalScrollBar(scroll_bar != nullptr ? scroll_bar : Create<ScrollBar>());
    InitializeLayouter();

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

    layouter_.reset();
    InitializeHorizontalScrollBar(scroll_bar != nullptr ? scroll_bar : Create<ScrollBar>());
    InitializeLayouter();

    HorizontalScrollBarChange(previous_scroll_bar);
    NeedRelayout();
}


void ScrollableControl::SetScrollContentControl(const std::shared_ptr<Control>& control) {

    auto previous_control = scroll_content_control_;
    if (previous_control == control) {
        return;
    }

    UpdateGuard update_guard(*this);

    scroll_container_control_->RemoveChild(previous_control);

    layouter_.reset();
    InitializeScrollContentControl(control != nullptr? control : Create<Control>());
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


void ScrollableControl::MouseWheel(const Point& position, const MouseWheelMessage& message) {

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
}


const Rect ScrollableControl::GetVisibleScrollContentRect() const {

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