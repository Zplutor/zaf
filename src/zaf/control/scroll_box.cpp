#include <zaf/control/scroll_box.h>
#include <zaf/base/as.h>
#include <zaf/control/control_object.h>
#include <zaf/control/internal/scroll_box/general_scroll_layouter.h>
#include <zaf/control/internal/scroll_box/self_scroll_layouter.h>
#include <zaf/control/rich_edit.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/self_scroll_control.h>
#include <zaf/control/text_box.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>
#include <zaf/rx/disposable.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {
namespace {

std::shared_ptr<Control> CreateDefaultScrollContentControl() {
    auto control = Create<Control>();
    control->SetBackgroundColor(Color::Transparent());
    return control;
}

}

ZAF_OBJECT_IMPL(ScrollBox)


ScrollBox::ScrollBox() :
    self_scrolling_control_(nullptr) {

}


ScrollBox::~ScrollBox() {

    //Layouter must be the first member to destroy for unregistering 
    //events from other members.
    layouter_.reset();
}


void ScrollBox::Initialize() {

    __super::Initialize();

    SetBorder(Frame(1));

    SetBorderColor(Color::Black());

    //The container control should be added before the scroll bars to position the container below 
    //the scroll bars. This ensures that overlay scroll bars are not overlapped by the container.
    viewport_control_ = Create<Control>();
    viewport_control_->SetBackgroundColor(Color::Transparent());
    AddChild(viewport_control_);

    InitializeVerticalScrollBar(Create<ScrollBar>());
    InitializeHorizontalScrollBar(Create<ScrollBar>());
    InitializeScrollBarCorner(Create<Control>());

    InitializeScrollContentControl(CreateDefaultScrollContentControl());
    InitializeLayouter();
}


void ScrollBox::InitializeScrollContentControl(const std::shared_ptr<Control>& control) {

    scroll_content_control_ = control;

    //Note: It's known that cached painting might cause blurry text rendering. We have to disable
    //it until a solution is found.
    /*
    scroll_content_control_->SetIsCachedPaintingEnabled([&control]() {
        return !As<RichEdit>(control) && !As<TextBox>(control);
    }());
    */

    viewport_control_->AddChild(scroll_content_control_);

    self_scrolling_control_ = dynamic_cast<SelfScrollControl*>(control.get());
    if (self_scrolling_control_ != nullptr) {

        self_scrolling_control_->SetAllowVerticalScroll(AllowVerticalScroll());
        self_scrolling_control_->SetAllowHorizontalScroll(AllowHorizontalScroll());
        self_scrolling_control_->SetAutoHideScrollBars(AutoHideScrollBars());   
    }

    scroll_content_rect_change_subscription_ = scroll_content_control_->RectChangedEvent().Subscribe(
        std::bind(&ScrollBox::OnScrollContentRectChange, this, std::placeholders::_1));
}


void ScrollBox::InitializeVerticalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    vertical_scroll_bar_ = scroll_bar;
    AddChild(vertical_scroll_bar_);
}


void ScrollBox::InitializeHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    horizontal_scroll_bar_ = scroll_bar;
    horizontal_scroll_bar_->SetIsHorizontal(true);
    AddChild(horizontal_scroll_bar_);
}


void ScrollBox::InitializeScrollBarCorner(const std::shared_ptr<Control>& corner) {

    scroll_bar_corner_ = corner;
    AddChild(scroll_bar_corner_);
}


void ScrollBox::InitializeLayouter() {

    if (self_scrolling_control_ == nullptr) {
        layouter_ = std::make_unique<internal::GeneralScrollLayouter>(this);
    }
    else {
        layouter_ = std::make_unique<internal::SelfScrollLayouter>(this);
    }
}


void ScrollBox::Layout(const zaf::Rect& previous_rect) {

    if (layouter_) {
        layouter_->Layout();
    }
}


void ScrollBox::OnScrollContentRectChange(const RectChangedInfo& event_info) {

    if (As<Control>(event_info.Source())->Size() != event_info.PreviousRect().size) {
        NeedRelayout();
    }
}


bool ScrollBox::AllowVerticalScroll() const {
    return allow_vertical_scroll_;
}


void ScrollBox::SetAllowVerticalScroll(bool allow_scroll) {

    allow_vertical_scroll_ = allow_scroll;

    if (self_scrolling_control_ != nullptr) {
        self_scrolling_control_->SetAllowVerticalScroll(allow_scroll);
    }

    NeedRelayout();
}


bool ScrollBox::AllowHorizontalScroll() const {
    return allow_horizontal_scroll_;
}


void ScrollBox::SetAllowHorizontalScroll(bool allow_scroll) {

    allow_horizontal_scroll_ = allow_scroll;

    if (self_scrolling_control_ != nullptr) {
        self_scrolling_control_->SetAllowHorizontalScroll(allow_scroll);
    }

    NeedRelayout();
}


bool ScrollBox::AutoHideScrollBars() const {
    return auto_hide_scroll_bars_;
}


void ScrollBox::SetAutoHideScrollBars(bool auto_hide) {

    auto_hide_scroll_bars_ = auto_hide;

    if (self_scrolling_control_ != nullptr) {
        self_scrolling_control_->SetAutoHideScrollBars(auto_hide);
    }

    NeedRelayout();
}


bool ScrollBox::UseOverlayScrollBars() const {
    return use_overlay_scroll_bars_;
}


void ScrollBox::SetUseOverlayScrollBars(bool use) {

    if (use_overlay_scroll_bars_ == use) {
        return;
    }

    use_overlay_scroll_bars_ = use;
    NeedRelayout();
}


bool ScrollBox::AutoScrollBarLargeChange() const {
    return auto_scroll_bar_large_change_;
}


void ScrollBox::SetAutoScrollBarLargeChange(bool value) {

    auto_scroll_bar_large_change_ = value;
    NeedRelayout();
}


void ScrollBox::SetVerticalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

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


void ScrollBox::SetHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

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


void ScrollBox::SetScrollBarCorner(const std::shared_ptr<Control>& control) {

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


void ScrollBox::SetScrollContent(const std::shared_ptr<Control>& control) {

    auto previous_control = scroll_content_control_;
    if (previous_control == control) {
        return;
    }

    auto update_guard = this->BeginUpdate();

    if (scroll_content_rect_change_subscription_) {
        scroll_content_rect_change_subscription_->Dispose();
    }
    viewport_control_->RemoveChild(previous_control);

    //Destroy layouter first for unregistering events before changing scroll content control.
    layouter_.reset();
    InitializeScrollContentControl(control ? control : CreateDefaultScrollContentControl());
    InitializeLayouter();

    OnScrollContentChanged(previous_control);
    NeedRelayout();
}


bool ScrollBox::AutoScrollContentWidth() const {
    return auto_scroll_content_width_;
}


void ScrollBox::SetAutoScrollContentWidth(bool value) {

    auto_scroll_content_width_ = value;
    NeedRelayout();
}


bool ScrollBox::AutoScrollContentHeight() const {
    return auto_scroll_content_height_;
}


void ScrollBox::SetAutoScrollContentHeight(bool value) {

    auto_scroll_content_height_ = value;
    NeedRelayout();
}


void ScrollBox::SetAutoScrollContentSize(bool value) {

    auto update_guard = BeginUpdate();
    SetAutoScrollContentWidth(value);
    SetAutoScrollContentHeight(value);
}


float ScrollBox::VerticalScrollBarThickness() const {
    return vertical_scroll_bar_thickness_;
}

void ScrollBox::SetVerticalScrollBarThickness(float thickness) {
    vertical_scroll_bar_thickness_ = thickness;
    NeedRelayout();
}


float ScrollBox::HorizontalScrollBarThickness() const {
    return horizontal_scroll_bar_thickness_;
}

void ScrollBox::SetHorizontalScrollBarThickness(float thickness) {
    horizontal_scroll_bar_thickness_ = thickness;
    NeedRelayout();
}


void ScrollBox::SetScrollBarThickness(float thickness) {
    auto update_guard = BeginUpdate();
    SetVerticalScrollBarThickness(thickness);
    SetHorizontalScrollBarThickness(thickness);
}


void ScrollBox::OnMouseWheel(const MouseWheelInfo& event_info) {

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
        event_info.MarkAsHandled();
    }
}


zaf::Rect ScrollBox::ViewportRect() const noexcept {
    auto rect_in_content = viewport_control_->Rect();
    rect_in_content.AddOffset(ContentRect().position);
    return rect_in_content;
}


zaf::Rect ScrollBox::GetVisibleScrollContentRect() const {

    if (self_scrolling_control_ != nullptr) {
        return viewport_control_->Rect();
    }

    zaf::Rect rect;
    rect.position = scroll_content_control_->Position();
    rect.position.x *= -1;
    rect.position.y *= -1;
    rect.size = viewport_control_->Size();
    return rect;
}


zaf::Point ScrollBox::TranslateToScrollContent(const zaf::Point& position) const {

    auto position_in_container = viewport_control_->TranslateFromParent(position);
    return scroll_content_control_->TranslateFromParent(position_in_container);
}


void ScrollBox::ScrollToScrollContentPosition(const Point& position) {

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


void ScrollBox::ScrollToTop() {
    const auto& vertical_scroll_bar = VerticalScrollBar();
    vertical_scroll_bar->SetValue(vertical_scroll_bar->MinValue()); 
}


void ScrollBox::ScrollToBottom() {
    const auto& vertical_scroll_bar = VerticalScrollBar();
    vertical_scroll_bar->SetValue(vertical_scroll_bar->MaxValue());
}


void ScrollBox::ScrollToLeft() {
    const auto& horizontal_scroll_bar = HorizontalScrollBar();
    horizontal_scroll_bar->SetValue(horizontal_scroll_bar->MinValue());
}


void ScrollBox::ScrollToRight() {
    const auto& horizontal_scroll_bar = HorizontalScrollBar();
    horizontal_scroll_bar->SetValue(horizontal_scroll_bar->MaxValue());
}


}