#include <zaf/control/scrollable_control.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/self_scrolling_control.h>
#include <zaf/creation.h>
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

    InitializeVerticalScrollBar(Create<ScrollBar>());
    InitializeHorizontalScrollBar(Create<ScrollBar>());

    scroll_container_control_ = Create<Control>();
    AddChild(scroll_container_control_);

    InitializeScrolledControl(Create<Control>());
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
    scroll_bar->GetScrollEvent().AddListenerWithTag(std::bind(&ScrollableControl::ScrollBarScroll, this, std::placeholders::_1), this);
}


void ScrollableControl::InitializeScrolledControl(const std::shared_ptr<Control>& control) {

    scrolled_control_ = control;
    scroll_container_control_->AddChild(scrolled_control_);

    self_scrolling_control_ = dynamic_cast<SelfScrollingControl*>(control.get());
    if (self_scrolling_control_ != nullptr) {

        self_scrolling_control_->SetAllowVerticalScroll(AllowVerticalScroll());
        self_scrolling_control_->SetAllowHorizontalScroll(AllowHorizontalScroll());
        self_scrolling_control_->SetAutoHideScrollBars(AutoHideScrollBars());

        self_scrolling_control_->GetScrollBarChangeEvent().AddListenerWithTag(
            std::bind(&ScrollableControl::SelfScrollingControlScrollBarChange, this, std::placeholders::_1), 
            this
        );

        self_scrolling_control_->GetScrollValuesChangeEvent().AddListenerWithTag(
            std::bind(&ScrollableControl::SelfScrollingControlScrollValuesChange, this, std::placeholders::_1, std::placeholders::_2),
            this
        );
    }
}


void ScrollableControl::Layout(const Rect& previous_rect) {

    if (self_scrolling_control_ == nullptr) {
        LayoutWithGeneralScrolledControl();
    }
    else {
        LayoutWithSelfScrollingControl();
    }
}


void ScrollableControl::LayoutWithGeneralScrolledControl() {

    bool can_show_vertical_scroll_bar = false;
    bool can_show_horizontal_scroll_bar = false;
    CanShowScrollBars(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);

    LayoutScrollBars(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);
    LayoutScrollContainerControl(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);
    LayoutScrolledControlSize(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);
    AdjustScrollBarValuesWithGeneralScrolledControl();
}


void ScrollableControl::CanShowScrollBars(bool& can_show_vertical_scroll_bar, bool& can_show_horizontal_scroll_bar) const {

    Rect content_rect = GetContentRect();
    float content_width = content_rect.size.width;
    float content_height = content_rect.size.height;

    bool allow_vertical_scroll = AllowVerticalScroll();
    bool allow_horizontal_scroll = AllowHorizontalScroll();

    bool auto_hide_scroll_bars = AutoHideScrollBars();

    auto determine = [auto_hide_scroll_bars](bool allow, float expected, float actual) {
        return allow && (!auto_hide_scroll_bars || (expected > actual));
    };

    can_show_vertical_scroll_bar = determine(allow_vertical_scroll, expected_scroll_area_size_.height, content_height);
    can_show_horizontal_scroll_bar = determine(allow_horizontal_scroll, expected_scroll_area_size_.width, content_width);

    float scroll_bar_thickness = GetScrollBarThickness();

    if (can_show_vertical_scroll_bar) {
        can_show_horizontal_scroll_bar = determine(
            allow_horizontal_scroll,
            expected_scroll_area_size_.width,
            content_width - scroll_bar_thickness
        );
    }

    if (can_show_horizontal_scroll_bar) {
        can_show_vertical_scroll_bar = determine(
            allow_vertical_scroll,
            expected_scroll_area_size_.height,
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


void ScrollableControl::LayoutScrolledControlSize(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar) {

    const Size& min_size = scroll_container_control_->GetSize();

    Rect new_rect = scrolled_control_->GetRect();
    new_rect.size = expected_scroll_area_size_;

    if (! can_show_horizontal_scroll_bar || new_rect.size.width < min_size.width) {
        new_rect.size.width = min_size.width;
        new_rect.position.x = 0;
    }

    if (! can_show_vertical_scroll_bar || new_rect.size.height < min_size.height) {
        new_rect.size.height = min_size.height;
        new_rect.position.y = 0;
    }

    scrolled_control_->SetRect(new_rect);
}


void ScrollableControl::AdjustScrollBarValuesWithGeneralScrolledControl() {

    const Size& content_size = scrolled_control_->GetSize();
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
    scrolled_control_->SetRect(Rect(Point(), scroll_container_control_->GetSize()));

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

    if (vertical_scroll_bar_ != nullptr) {
        vertical_scroll_bar_->GetScrollEvent().RemoveListenersWithTag(this);
        RemoveChild(vertical_scroll_bar_);
    }

    InitializeVerticalScrollBar(scroll_bar);
    NeedRelayout();
}


void ScrollableControl::SetHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    if (horizontal_scroll_bar_ != nullptr) {
        horizontal_scroll_bar_->GetScrollEvent().RemoveListenersWithTag(this);
        RemoveChild(horizontal_scroll_bar_);
    }

    InitializeHorizontalScrollBar(scroll_bar);
    NeedRelayout();
}


void ScrollableControl::SetScrolledControl(const std::shared_ptr<Control>& control) {

    if (scrolled_control_ != nullptr) {
        scroll_container_control_->RemoveChild(scrolled_control_);
    }

    if (self_scrolling_control_ != nullptr) {
        self_scrolling_control_->GetScrollBarChangeEvent().RemoveListenersWithTag(this);
        self_scrolling_control_->GetScrollValuesChangeEvent().RemoveListenersWithTag(this);
    }

    InitializeScrolledControl(control);
    NeedRelayout();
}


float ScrollableControl::GetScrollBarThickness() const {

    auto thickness = GetPropertyMap().TryGetProperty<float>(kScrollBarThicknessPropertyName);
    if (thickness != nullptr) {
        return *thickness;
    }
    else {
        return 12;
    }
}


void ScrollableControl::SetScrollBarThickness(float thickness) {

    GetPropertyMap().SetProperty(kScrollBarThicknessPropertyName, thickness);
    NeedRelayout();
}


void ScrollableControl::SetScrollAreaSize(const Size& size) {

    expected_scroll_area_size_ = size;
    NeedRelayout();
}


void ScrollableControl::MouseWheel(const Point& position, const MouseWheelMessage& message) {

    std::shared_ptr<ScrollBar> scroll_bar;

    if (message.is_horizontal) {
        scroll_bar = horizontal_scroll_bar_;   
    }
    else {
        scroll_bar = vertical_scroll_bar_;
    }

    if (scroll_bar->IsVisible() && scroll_bar->IsEnabled()) {
        scroll_bar->Wheel(message.distance);
    }
}


void ScrollableControl::ScrollBarScroll(const std::shared_ptr<ScrollBar>& scroll_bar) {

    if (self_scrolling_control_ == nullptr) {
        ScrollGeneralScrolledControl(scroll_bar);
    }
    else {
        ScrollSelfScrollingControl(scroll_bar);
    }
}


void ScrollableControl::ScrollGeneralScrolledControl(const std::shared_ptr<ScrollBar>& scroll_bar) {

    Rect content_rect = scrolled_control_->GetRect();

    if (scroll_bar == vertical_scroll_bar_) {
        content_rect.position.y = static_cast<float>(-scroll_bar->GetValue());
    }
    else if (scroll_bar == horizontal_scroll_bar_) {
        content_rect.position.x = static_cast<float>(-scroll_bar->GetValue());
    }

    scrolled_control_->SetRect(content_rect);
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


const Rect ScrollableControl::GetVisibleScrollAreaRect() const {

    if (self_scrolling_control_ != nullptr) {
        return scroll_container_control_->GetRect();
    }

    Rect rect;
    rect.position = scrolled_control_->GetPosition();
    rect.position.x *= -1;
    rect.position.y *= -1;
    rect.size = scroll_container_control_->GetSize();
    return rect;
}


void ScrollableControl::ScrollToScrollAreaPosition(const Point& position) {

    if (self_scrolling_control_ != nullptr) {
        return;
    }

    if (AllowHorizontalScroll() && horizontal_scroll_bar_->IsEnabled()) {
        horizontal_scroll_bar_->SetValue(position.x);
    }

    if (AllowVerticalScroll() && vertical_scroll_bar_->IsEnabled()) {
        vertical_scroll_bar_->SetValue(position.y);
    }
}


}