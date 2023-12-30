#include <zaf/control/scrollable_control.h>
#include <zaf/base/as.h>
#include <zaf/control/internal/scrollable_control/general_scrolling_layouter.h>
#include <zaf/control/internal/scrollable_control/self_scrolling_layouter.h>
#include <zaf/control/rich_edit.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scrollable_control_parser.h>
#include <zaf/control/self_scrolling_control.h>
#include <zaf/control/text_box.h>
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

}


ZAF_DEFINE_TYPE(ScrollableControl)
ZAF_DEFINE_TYPE_PARSER(ScrollableControlParser)
ZAF_DEFINE_TYPE_PROPERTY(AllowVerticalScroll)
ZAF_DEFINE_TYPE_PROPERTY(AllowHorizontalScroll)
ZAF_DEFINE_TYPE_PROPERTY(AutoScrollBarLargeChange)
ZAF_DEFINE_TYPE_PROPERTY(AutoScrollContentHeight)
ZAF_DEFINE_TYPE_PROPERTY(AutoScrollContentSize)
ZAF_DEFINE_TYPE_PROPERTY(AutoScrollContentWidth)
ZAF_DEFINE_TYPE_PROPERTY(AutoHideScrollBars)
ZAF_DEFINE_TYPE_PROPERTY(ScrollBarThickness)
ZAF_DEFINE_TYPE_PROPERTY(UseOverlayScrollBars)
ZAF_DEFINE_TYPE_PROPERTY_DYNAMIC(HorizontalScrollBar)
ZAF_DEFINE_TYPE_PROPERTY(HorizontalScrollBarThickness)
ZAF_DEFINE_TYPE_PROPERTY_DYNAMIC(ScrollBarCorner)
ZAF_DEFINE_TYPE_PROPERTY_DYNAMIC(VerticalScrollBar)
ZAF_DEFINE_TYPE_PROPERTY(VerticalScrollBarThickness)
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

    //The container control should be added before the scroll bars to position the container below 
    //the scroll bars. This ensures that overlay scroll bars are not overlapped by the container.
    scroll_container_control_ = Create<Control>();
    scroll_container_control_->SetBackgroundColor(Color::Transparent());
    AddChild(scroll_container_control_);

    InitializeVerticalScrollBar(Create<ScrollBar>());
    InitializeHorizontalScrollBar(Create<ScrollBar>());
    InitializeScrollBarCorner(Create<Control>());

    InitializeScrollContentControl(CreateDefaultScrollContentControl());
    InitializeLayouter();
}


void ScrollableControl::InitializeScrollContentControl(const std::shared_ptr<Control>& control) {

    scroll_content_control_ = control;

    //Note: It's known that cached painting might cause blurry text rendering. We have to disable
    //it until a solution is found.
    /*
    scroll_content_control_->SetIsCachedPaintingEnabled([&control]() {
        return !As<RichEdit>(control) && !As<TextBox>(control);
    }());
    */

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
        layouter_->Layout();
    }
}


void ScrollableControl::OnScrollContentRectChange(const RectChangedInfo& event_info) {

    if (As<Control>(event_info.Source())->Size() != event_info.PreviousRect().size) {
        NeedRelayout();
    }
}


bool ScrollableControl::AllowVerticalScroll() const {
    return allow_vertical_scroll_;
}


void ScrollableControl::SetAllowVerticalScroll(bool allow_scroll) {

    allow_vertical_scroll_ = allow_scroll;

    if (self_scrolling_control_ != nullptr) {
        self_scrolling_control_->SetAllowVerticalScroll(allow_scroll);
    }

    NeedRelayout();
}


bool ScrollableControl::AllowHorizontalScroll() const {
    return allow_horizontal_scroll_;
}


void ScrollableControl::SetAllowHorizontalScroll(bool allow_scroll) {

    allow_horizontal_scroll_ = allow_scroll;

    if (self_scrolling_control_ != nullptr) {
        self_scrolling_control_->SetAllowHorizontalScroll(allow_scroll);
    }

    NeedRelayout();
}


bool ScrollableControl::AutoHideScrollBars() const {
    return auto_hide_scroll_bars_;
}


void ScrollableControl::SetAutoHideScrollBars(bool auto_hide) {

    auto_hide_scroll_bars_ = auto_hide;

    if (self_scrolling_control_ != nullptr) {
        self_scrolling_control_->SetAutoHideScrollBars(auto_hide);
    }

    NeedRelayout();
}


bool ScrollableControl::UseOverlayScrollBars() const {
    return use_overlay_scroll_bars_;
}


void ScrollableControl::SetUseOverlayScrollBars(bool use) {

    if (use_overlay_scroll_bars_ == use) {
        return;
    }

    use_overlay_scroll_bars_ = use;
    NeedRelayout();
}


bool ScrollableControl::AutoScrollBarLargeChange() const {
    return auto_scroll_bar_large_change_;
}


void ScrollableControl::SetAutoScrollBarLargeChange(bool value) {

    auto_scroll_bar_large_change_ = value;
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
    return auto_scroll_content_width_;
}


void ScrollableControl::SetAutoScrollContentWidth(bool value) {

    auto_scroll_content_width_ = value;
    NeedRelayout();
}


bool ScrollableControl::AutoScrollContentHeight() const {
    return auto_scroll_content_height_;
}


void ScrollableControl::SetAutoScrollContentHeight(bool value) {

    auto_scroll_content_height_ = value;
    NeedRelayout();
}


void ScrollableControl::SetAutoScrollContentSize(bool value) {

    auto update_guard = BeginUpdate();
    SetAutoScrollContentWidth(value);
    SetAutoScrollContentHeight(value);
}


float ScrollableControl::VerticalScrollBarThickness() const {
    return vertical_scroll_bar_thickness_;
}

void ScrollableControl::SetVerticalScrollBarThickness(float thickness) {
    vertical_scroll_bar_thickness_ = thickness;
    NeedRelayout();
}


float ScrollableControl::HorizontalScrollBarThickness() const {
    return horizontal_scroll_bar_thickness_;
}

void ScrollableControl::SetHorizontalScrollBarThickness(float thickness) {
    horizontal_scroll_bar_thickness_ = thickness;
    NeedRelayout();
}


void ScrollableControl::SetScrollBarThickness(float thickness) {
    auto update_guard = BeginUpdate();
    SetVerticalScrollBarThickness(thickness);
    SetHorizontalScrollBarThickness(thickness);
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
        event_info.MarkAsHandled();
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