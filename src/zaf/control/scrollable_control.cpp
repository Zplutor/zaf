#include <zaf/control/scrollable_control.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/self_scrolling_control.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {

static const wchar_t* const kAllowHorizontalScrollPropertyName = L"AllowHorizontalScroll";
static const wchar_t* const kAllowVerticalScrollPropertyName = L"AllowVerticalScroll";
static const wchar_t* const kAutoHideScrollBarsPropertyName = L"AutoHideScrollBars";
static const wchar_t* const kScrollBarThicknessPropertyName = L"ScrollBarThickness";

ScrollableControl::ScrollableControl() :
    self_scrolling_control_(nullptr),
    is_self_scrolling_(false) {

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

    InitializeScrollContentControl(Create<Control>());
}


void ScrollableControl::InitializeVerticalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    vertical_scroll_bar_ = scroll_bar;
    InitializeScrollBar(vertical_scroll_bar_);
}


void ScrollableControl::InitializeHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    horizontal_scroll_bar_ = scroll_bar;
    horizontal_scroll_bar_->SetIsHorizontal(true);
    InitializeScrollBar(horizontal_scroll_bar_);
}


void ScrollableControl::InitializeScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    AddChild(scroll_bar);
    scroll_bar->GetScrollEvent().AddListenerWithTag(
        reinterpret_cast<std::uintptr_t>(this),
        std::bind(&ScrollableControl::ScrollBarScroll, this, std::placeholders::_1));
}


void ScrollableControl::InitializeScrollContentControl(const std::shared_ptr<Control>& control) {

    scroll_content_control_ = control;
    scroll_container_control_->AddChild(scroll_content_control_);

    self_scrolling_control_ = dynamic_cast<SelfScrollingControl*>(control.get());
    if (self_scrolling_control_ != nullptr) {

        self_scrolling_control_->SetAllowVerticalScroll(AllowVerticalScroll());
        self_scrolling_control_->SetAllowHorizontalScroll(AllowHorizontalScroll());
        self_scrolling_control_->SetAutoHideScrollBars(AutoHideScrollBars());

        self_scrolling_control_->GetScrollBarChangeEvent().AddListenerWithTag(
            reinterpret_cast<std::uintptr_t>(this),
            std::bind(&ScrollableControl::SelfScrollingControlScrollBarChange, this, std::placeholders::_1)
        );

        self_scrolling_control_->GetScrollValuesChangeEvent().AddListenerWithTag(
            reinterpret_cast<std::uintptr_t>(this),
            std::bind(&ScrollableControl::SelfScrollingControlScrollValuesChange, this, std::placeholders::_1, std::placeholders::_2)
        );
    }
}


void ScrollableControl::Layout(const Rect& previous_rect) {

    if (self_scrolling_control_ == nullptr) {
        LayoutWithGeneralScrollContentControl();
    }
    else {
        LayoutWithSelfScrollingControl();
    }
}


void ScrollableControl::LayoutWithGeneralScrollContentControl() {

    bool can_show_vertical_scroll_bar = false;
    bool can_show_horizontal_scroll_bar = false;
    CanShowScrollBars(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);

    LayoutScrollBars(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);
    LayoutScrollContainerControl(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);
    LayoutScrollContentControlSize(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);
    AdjustScrollBarValuesWithGeneralScrollContentControl();
}


void ScrollableControl::CanShowScrollBars(bool& can_show_vertical_scroll_bar, bool& can_show_horizontal_scroll_bar) const {

    Rect content_rect = GetContentRect();
    float content_width = content_rect.size.width;
    float content_height = content_rect.size.height;

    Size expected_scroll_content_control_size = GetExpectedScrollContentControlSize();

    bool allow_vertical_scroll = AllowVerticalScroll();
    bool allow_horizontal_scroll = AllowHorizontalScroll();

    bool auto_hide_scroll_bars = AutoHideScrollBars();

    auto determine = [auto_hide_scroll_bars](bool allow, float expected, float actual) {
        return allow && (!auto_hide_scroll_bars || (expected > actual));
    };

    can_show_vertical_scroll_bar = determine(allow_vertical_scroll, expected_scroll_content_control_size.height, content_height);
    can_show_horizontal_scroll_bar = determine(allow_horizontal_scroll, expected_scroll_content_control_size.width, content_width);

    float scroll_bar_thickness = GetScrollBarThickness();

    if (can_show_vertical_scroll_bar) {
        can_show_horizontal_scroll_bar = determine(
            allow_horizontal_scroll,
            expected_scroll_content_control_size.width,
            content_width - scroll_bar_thickness
        );
    }

    if (can_show_horizontal_scroll_bar) {
        can_show_vertical_scroll_bar = determine(
            allow_vertical_scroll,
            expected_scroll_content_control_size.height,
            content_height - scroll_bar_thickness
        );
    }
}


void ScrollableControl::LayoutScrollBars(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar) {

    vertical_scroll_bar_->SetIsVisible(can_show_vertical_scroll_bar);
    horizontal_scroll_bar_->SetIsVisible(can_show_horizontal_scroll_bar);

    auto content_size = GetContentSize();
    float scroll_bar_thickness = GetScrollBarThickness();

    if (can_show_vertical_scroll_bar) {

	    Rect vertical_scroll_bar_rect(
            content_size.width - scroll_bar_thickness,
		    0,
            scroll_bar_thickness,
            content_size.height - (can_show_horizontal_scroll_bar ? scroll_bar_thickness : 0)
	    );
	    vertical_scroll_bar_->SetRect(vertical_scroll_bar_rect);
    }

    if (can_show_horizontal_scroll_bar) {

	    Rect horizontal_scroll_bar_rect(
		    0,
            content_size.height - scroll_bar_thickness,
            content_size.width - (can_show_vertical_scroll_bar ? scroll_bar_thickness : 0),
            scroll_bar_thickness
	    );
	    horizontal_scroll_bar_->SetRect(horizontal_scroll_bar_rect);
    }
}


void ScrollableControl::LayoutScrollContainerControl(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar) {

	Rect rect(Point(), GetContentSize());

    if (can_show_vertical_scroll_bar) {
	    rect.size.width -= vertical_scroll_bar_->GetWidth();
    }

    if (can_show_horizontal_scroll_bar) {
	    rect.size.height -= horizontal_scroll_bar_->GetHeight();
    }

	scroll_container_control_->SetRect(rect);
}


void ScrollableControl::LayoutScrollContentControlSize(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar) {

    const Size& min_size = scroll_container_control_->GetSize();

    Rect new_rect = scroll_content_control_->GetRect();
    new_rect.size = GetExpectedScrollContentControlSize();

    if (! can_show_horizontal_scroll_bar || new_rect.size.width < min_size.width) {
        new_rect.size.width = min_size.width;
        new_rect.position.x = 0;
    }

    if (! can_show_vertical_scroll_bar || new_rect.size.height < min_size.height) {
        new_rect.size.height = min_size.height;
        new_rect.position.y = 0;
    }

    scroll_content_control_->SetRect(new_rect);
}


void ScrollableControl::AdjustScrollBarValuesWithGeneralScrollContentControl() {

    const Size& content_size = scroll_content_control_->GetSize();
    const Size& content_container_size = scroll_container_control_->GetSize();

    int vertical_scroll_value_range = static_cast<int>(content_size.height - content_container_size.height);
    vertical_scroll_bar_->SetValueRange(0, vertical_scroll_value_range);
    vertical_scroll_bar_->SetIsEnabled(vertical_scroll_value_range != 0);

    int horizontal_scroll_value_range = static_cast<int>(content_size.width - content_container_size.width);
    horizontal_scroll_bar_->SetValueRange(0, horizontal_scroll_value_range);
    horizontal_scroll_bar_->SetIsEnabled(horizontal_scroll_value_range != 0);
}


void ScrollableControl::LayoutWithSelfScrollingControl() {

    bool can_show_vertical_scroll_bar = AllowVerticalScroll() && self_scrolling_control_->CanShowVerticalScrollBar();
    bool can_show_horizontal_scroll_bar = AllowHorizontalScroll() && self_scrolling_control_->CanShowHorizontalScrollBar();

    LayoutScrollBars(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);
    LayoutScrollContainerControl(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);
    scroll_content_control_->SetRect(Rect(Point(), scroll_container_control_->GetSize()));

    AdjustScrollBarValueWithSelfScrollingControl(true);
    AdjustScrollBarValueWithSelfScrollingControl(false);
}


void ScrollableControl::AdjustScrollBarValueWithSelfScrollingControl(bool is_horizontal) {

    int current_value = 0;
    int min_value = 0;
    int max_value = 0;
    std::shared_ptr<ScrollBar> scroll_bar;

    if (is_horizontal) {
        self_scrolling_control_->GetHorizontalScrollValues(current_value, min_value, max_value);
        scroll_bar = horizontal_scroll_bar_;
    }
    else {
        self_scrolling_control_->GetVerticalScrollValues(current_value, min_value, max_value);
        scroll_bar = vertical_scroll_bar_;
    }

    scroll_bar->SetValueRange(min_value, max_value);
    scroll_bar->SetValue(current_value);
}


Size ScrollableControl::GetExpectedScrollContentControlSize() const {

    const auto& scroll_content_control = GetScrollContentControl();
    const auto& padding = scroll_content_control->GetPadding();
    const auto& border = scroll_content_control->GetBorder();

    Size size = expected_scroll_content_size_;
    size.width += padding.left + padding.right + border.left + border.right;
    size.height += padding.top + padding.bottom + border.top + border.bottom;
    return size;
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

    previous_scroll_bar->GetScrollEvent().RemoveListenersWithTag(reinterpret_cast<std::uintptr_t>(this));
    RemoveChild(previous_scroll_bar);

    InitializeVerticalScrollBar(scroll_bar != nullptr ? scroll_bar : Create<ScrollBar>());
    VerticalScrollBarChange(previous_scroll_bar);
    NeedRelayout();
}


void ScrollableControl::SetHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    auto previous_scroll_bar = horizontal_scroll_bar_;
    if (previous_scroll_bar == scroll_bar) {
        return;
    }

    previous_scroll_bar->GetScrollEvent().RemoveListenersWithTag(reinterpret_cast<std::uintptr_t>(this));
    RemoveChild(previous_scroll_bar);

    InitializeHorizontalScrollBar(scroll_bar != nullptr ? scroll_bar : Create<ScrollBar>());
    HorizontalScrollBarChange(previous_scroll_bar);
    NeedRelayout();
}


void ScrollableControl::SetScrollContentControl(const std::shared_ptr<Control>& control) {

    if (scroll_content_control_ != nullptr) {
        scroll_container_control_->RemoveChild(scroll_content_control_);
    }

    if (self_scrolling_control_ != nullptr) {
        self_scrolling_control_->GetScrollBarChangeEvent().RemoveListenersWithTag(reinterpret_cast<std::uintptr_t>(this));
        self_scrolling_control_->GetScrollValuesChangeEvent().RemoveListenersWithTag(reinterpret_cast<std::uintptr_t>(this));
    }

    InitializeScrollContentControl(control != nullptr? control : Create<Control>());
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


void ScrollableControl::ScrollBarScroll(const std::shared_ptr<ScrollBar>& scroll_bar) {

    if (self_scrolling_control_ == nullptr) {
        ScrollGeneralScrollContentControl(scroll_bar);
    }
    else {
        ScrollSelfScrollingControl(scroll_bar);
    }
}


void ScrollableControl::ScrollGeneralScrollContentControl(const std::shared_ptr<ScrollBar>& scroll_bar) {

    Rect content_rect = scroll_content_control_->GetRect();

    if (scroll_bar == vertical_scroll_bar_) {
        content_rect.position.y = static_cast<float>(-scroll_bar->GetValue());
    }
    else if (scroll_bar == horizontal_scroll_bar_) {
        content_rect.position.x = static_cast<float>(-scroll_bar->GetValue());
    }

    scroll_content_control_->SetRect(content_rect);
}


void ScrollableControl::ScrollSelfScrollingControl(const std::shared_ptr<ScrollBar>& scroll_bar) {

    if (is_self_scrolling_) {
        return;
    }

    int value = scroll_bar->GetValue();
    if (scroll_bar->IsHorizontal()) {
        self_scrolling_control_->HorizontallyScroll(value);
    }
    else {
        self_scrolling_control_->VerticallyScroll(value);
    }
}


void ScrollableControl::SelfScrollingControlScrollBarChange(SelfScrollingControl& self_scrolling_control) {

    LayoutWithSelfScrollingControl();

    vertical_scroll_bar_->SetIsEnabled(self_scrolling_control_->CanEnableVerticalScrollBar());
    horizontal_scroll_bar_->SetIsEnabled(self_scrolling_control_->CanEnableHorizontalScrollBar());
}


void ScrollableControl::SelfScrollingControlScrollValuesChange(SelfScrollingControl& self_scrolling_control, bool is_horizontal) {

    is_self_scrolling_ = true;
    AdjustScrollBarValueWithSelfScrollingControl(is_horizontal);
    is_self_scrolling_ = false;
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


}