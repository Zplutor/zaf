#include <zaf/control/scrollable_control.h>
#include <zaf/control/internal/scrollable_control/general_scrolling_layouter.h>
#include <zaf/control/internal/scrollable_control/self_scrolling_layouter.h>
#include <zaf/control/rich_edit.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/self_scrolling_control.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>
#include <zaf/object/type_definition.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {
namespace {

std::shared_ptr<Control> CreateDefaultScrollContentControl() {
    auto control = Create<Control>();
    control->SetBackgroundColor(Color::Transparent());
    return control;
}

constexpr const wchar_t* const kAllowHorizontalScrollPropertyName = L"AllowHorizontalScroll";
constexpr const wchar_t* const kAllowVerticalScrollPropertyName = L"AllowVerticalScroll";
constexpr const wchar_t* const kAutoScrollBarLargeChangePropertyName = L"AutoScrollBarLargeChange";
constexpr const wchar_t* const kAutoScrollContentWidthPropertyName = L"AutoScrollContentWidth";
constexpr const wchar_t* const kAutoScrollContentHeightPropertyName = L"AutoScrollContentHeight";
constexpr const wchar_t* const kAutoHideScrollBarsPropertyName = L"AutoHideScrollBars";
constexpr const wchar_t* const kScrollBarThicknessPropertyName = L"ScrollBarThickness";

}


ZAF_DEFINE_TYPE(ScrollableControl)
ZAF_DEFINE_TYPE_PROPERTY(AllowVerticalScroll)
ZAF_DEFINE_TYPE_PROPERTY(AllowHorizontalScroll)
ZAF_DEFINE_TYPE_PROPERTY(AutoScrollBarLargeChange)
ZAF_DEFINE_TYPE_PROPERTY(AutoScrollContentHeight)
ZAF_DEFINE_TYPE_PROPERTY(AutoScrollContentSize)
ZAF_DEFINE_TYPE_PROPERTY(AutoScrollContentWidth)
ZAF_DEFINE_TYPE_PROPERTY(AutoHideScrollBars)
ZAF_DEFINE_TYPE_PROPERTY(ScrollBarThickness)
ZAF_DEFINE_TYPE_PROPERTY_DYNAMIC(HorizontalScrollBar)
ZAF_DEFINE_TYPE_PROPERTY_DYNAMIC(ScrollBarCorner)
ZAF_DEFINE_TYPE_PROPERTY_DYNAMIC(VerticalScrollBar)
ZAF_DEFINE_TYPE_PROPERTY_DYNAMIC(ScrollContent)
ZAF_DEFINE_TYPE_END


ScrollableControl::ScrollableControl() :
    self_scrolling_control_(nullptr) {

}


ScrollableControl::~ScrollableControl() {

    //Layouter must be the first member to destroy for unregistering 
    //events from other members.
    layouter_.reset();
}


void ScrollableControl::Initialize() {

    SetBorder(Frame(1));

    SetBorderColorPicker([](const Control&) {
        return Color::Black();
    });

    InitializeVerticalScrollBar(Create<ScrollBar>());
    InitializeHorizontalScrollBar(Create<ScrollBar>());
    InitializeScrollBarCorner(Create<Control>());

    scroll_container_control_ = Create<Control>();
    scroll_container_control_->SetBackgroundColor(Color::Transparent());
    AddChild(scroll_container_control_);

    InitializeScrollContentControl(CreateDefaultScrollContentControl());
    InitializeLayouter();
}


void ScrollableControl::InitializeScrollContentControl(const std::shared_ptr<Control>& control) {

    scroll_content_control_ = control;

    //RichEdit cannot enable cached painting due to incorrect painting behavior.
    auto rich_edit = dynamic_cast<RichEdit*>(control.get());
    if (!rich_edit) {
        scroll_content_control_->SetIsCachedPaintingEnabled(true);
    }

    scroll_container_control_->AddChild(scroll_content_control_);

    self_scrolling_control_ = dynamic_cast<SelfScrollingControl*>(control.get());
    if (self_scrolling_control_ != nullptr) {

        self_scrolling_control_->SetAllowVerticalScroll(AllowVerticalScroll());
        self_scrolling_control_->SetAllowHorizontalScroll(AllowHorizontalScroll());
        self_scrolling_control_->SetAutoHideScrollBars(AutoHideScrollBars());   
    }

    scroll_content_rect_change_subscription_ = scroll_content_control_->RectChangedEvent().Subscribe(
        std::bind(&ScrollableControl::OnScrollContentRectChange, this, std::placeholders::_1));
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
        layouter_ = std::make_unique<internal::GeneralScrollingLayouter>(this);
    }
    else {
        layouter_ = std::make_unique<internal::SelfScrollingLayouter>(this);
    }
}


void ScrollableControl::Layout(const zaf::Rect& previous_rect) {

    if (layouter_) {

        is_layouting_ = true;
        layouter_->Layout();
        is_layouting_ = false;
    }
}


void ScrollableControl::OnScrollContentRectChange(const RectChangedInfo& event_info) {

    if (is_layouting_) {
        return;
    }

    if (As<Control>(event_info.Source())->Size() != event_info.PreviousRect().size) {
        NeedRelayout();
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


bool ScrollableControl::AutoScrollBarLargeChange() const {

    auto value = GetPropertyMap().TryGetProperty<bool>(
        kAutoScrollBarLargeChangePropertyName);

    if (value) {
        return *value;
    }
    else {
        return true;
    }
}


void ScrollableControl::SetAutoScrollBarLargeChange(bool value) {

    GetPropertyMap().SetProperty(kAutoScrollBarLargeChangePropertyName, value);
    NeedRelayout();
}


void ScrollableControl::SetVerticalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    auto previous_scroll_bar = vertical_scroll_bar_;
    if (previous_scroll_bar == scroll_bar) {
        return;
    }

    auto update_guard = this->BeginUpdate();

    RemoveChild(previous_scroll_bar);
    InitializeVerticalScrollBar(scroll_bar != nullptr ? scroll_bar : Create<ScrollBar>());

    layouter_->ScrollBarChange(false, previous_scroll_bar);
    OnVerticalScrollBarChanged(previous_scroll_bar);
    NeedRelayout();
}


void ScrollableControl::SetHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    auto previous_scroll_bar = horizontal_scroll_bar_;
    if (previous_scroll_bar == scroll_bar) {
        return;
    }

    auto update_guard = this->BeginUpdate();

    RemoveChild(previous_scroll_bar);
    InitializeHorizontalScrollBar(scroll_bar != nullptr ? scroll_bar : Create<ScrollBar>());

    layouter_->ScrollBarChange(true, previous_scroll_bar);
    OnHorizontalScrollBarChanged(previous_scroll_bar);
    NeedRelayout();
}


void ScrollableControl::SetScrollBarCorner(const std::shared_ptr<Control>& control) {

    auto previous_corner = scroll_bar_corner_;
    if (previous_corner == control) {
        return;
    }

    auto update_guard = this->BeginUpdate();

    RemoveChild(previous_corner);
    InitializeScrollBarCorner(control != nullptr ? control : Create<Control>());

    OnScrollBarCornerChanged(previous_corner);
    NeedRelayout();
}


void ScrollableControl::SetScrollContent(const std::shared_ptr<Control>& control) {

    auto previous_control = scroll_content_control_;
    if (previous_control == control) {
        return;
    }

    auto update_guard = this->BeginUpdate();

    scroll_content_rect_change_subscription_.Unsubscribe();
    scroll_container_control_->RemoveChild(previous_control);

    //Destroy layouter first for unregistering events before changing scroll content control.
    layouter_.reset();
    InitializeScrollContentControl(control ? control : CreateDefaultScrollContentControl());
    InitializeLayouter();

    OnScrollContentChanged(previous_control);
    NeedRelayout();
}


bool ScrollableControl::AutoScrollContentWidth() const {

    auto value = GetPropertyMap().TryGetProperty<bool>(
        kAutoScrollContentWidthPropertyName);

    if (value) {
        return *value;
    }
    return true;
}


void ScrollableControl::SetAutoScrollContentWidth(bool value) {

    GetPropertyMap().SetProperty(kAutoScrollContentWidthPropertyName, value);
    NeedRelayout();
}


bool ScrollableControl::AutoScrollContentHeight() const {

    auto value = GetPropertyMap().TryGetProperty<bool>(
        kAutoScrollContentHeightPropertyName);

    if (value) {
        return *value;
    }
    return true;
}


void ScrollableControl::SetAutoScrollContentHeight(bool value) {

    GetPropertyMap().SetProperty(kAutoScrollContentHeightPropertyName, value);
    NeedRelayout();
}


void ScrollableControl::SetAutoScrollContentSize(bool value) {

    auto update_guard = BeginUpdate();
    SetAutoScrollContentWidth(value);
    SetAutoScrollContentHeight(value);
}


float ScrollableControl::ScrollBarThickness() const {

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


void ScrollableControl::OnMouseWheel(const MouseWheelInfo& event_info) {

    __super::OnMouseWheel(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    std::shared_ptr<ScrollBar> scroll_bar;

    if (event_info.Message().IsHorizontalWheeling()) {
        scroll_bar = horizontal_scroll_bar_;   
    }
    else {
        scroll_bar = vertical_scroll_bar_;
    }

    if (scroll_bar->IsVisible() && scroll_bar->IsEnabled()) {
        scroll_bar->Wheel(event_info.Message().WheelingDistance());
    }

    event_info.MarkAsHandled();
}


void ScrollableControl::OnIsEnabledChanged() {

    __super::OnIsEnabledChanged();

    if (IsEnabled()) { 
        //Relayout to refresh scroll bars.
        NeedRelayout();
    }
}


zaf::Rect ScrollableControl::GetVisibleScrollContentRect() const {

    if (self_scrolling_control_ != nullptr) {
        return scroll_container_control_->Rect();
    }

    zaf::Rect rect;
    rect.position = scroll_content_control_->Position();
    rect.position.x *= -1;
    rect.position.y *= -1;
    rect.size = scroll_container_control_->Size();
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
    const auto& vertical_scroll_bar = VerticalScrollBar();
    vertical_scroll_bar->SetValue(vertical_scroll_bar->MinValue()); 
}


void ScrollableControl::ScrollDownToEnd() {
    const auto& vertical_scroll_bar = VerticalScrollBar();
    vertical_scroll_bar->SetValue(vertical_scroll_bar->MaxValue());
}


void ScrollableControl::ScrollLeftToBegin() {
    const auto& horizontal_scroll_bar = HorizontalScrollBar();
    horizontal_scroll_bar->SetValue(horizontal_scroll_bar->MinValue());
}


void ScrollableControl::ScrollRightToEnd() {
    const auto& horizontal_scroll_bar = HorizontalScrollBar();
    horizontal_scroll_bar->SetValue(horizontal_scroll_bar->MaxValue());
}


}