#include <zaf/control/combo_box.h>
#include <algorithm>
#include <zaf/base/as.h>
#include <zaf/base/range.h>
#include <zaf/control/control_object.h>
#include <zaf/control/button.h>
#include <zaf/control/internal/combo_box_drop_down_window.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include <zaf/control/rich_edit.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/dpi.h>
#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/internal/theme.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/window.h>

#ifdef min
#undef min
#endif

namespace zaf {

ZAF_OBJECT_IMPL(ComboBox);
ZAF_OBJECT_IMPL(ComboBoxDropDownListBox);
ZAF_OBJECT_IMPL(ComboBoxEditBox);


ComboBox::ComboBox() : 
    drop_down_list_box_action_(DropDownListBoxAction::CloseDropDownWindow),
    edit_box_action_(EditBoxAction::ChangeText),
    text_change_source_(TextChangeSource::None) {

}


ComboBox::~ComboBox() {

    UninitializeDropDownListBox();
    UninitializeEditBox();

    if (drop_down_window_) {
        drop_down_window_->Close();
    }
}


void ComboBox::Initialize() {

    __super::Initialize();

    SetBorder(Frame{ 1 });
    SetTextInset(Frame{ 3, 1, 1, 1 });
    SetDropDownButtonColor(Color::Black());
    SetParagraphAlignment(ParagraphAlignment::Center);

    drop_down_window_ = Create<internal::ComboBoxDropDownWindow>();
    drop_down_window_->SetIsPopup(true);
    drop_down_window_->SetHasBorder(false);
    drop_down_window_->SetActivateOption(ActivateOption::NoActivate);
    drop_down_window_->SetInitialRectStyle(InitialRectStyle::Custom);

    Subscriptions() += drop_down_window_->DestroyedEvent().Subscribe(
        std::bind(&ComboBox::DropDownWindowDestroy, this));

    drop_down_list_box_ = Create<ComboBoxDropDownListBox>();
    InitializeDropDownListBox();

    edit_box_ = Create<ComboBoxEditBox>();
    InitializeEditBox();
}


void ComboBox::InitializeDropDownListBox() {

    drop_down_list_box_->SetMouseMoveCallback(
        std::bind(&ComboBox::DropDownListBoxMouseMove, this, std::placeholders::_1));

    drop_down_list_box_subscription_ = drop_down_list_box_->SelectionChangedEvent().Subscribe(
        std::bind(&ComboBox::DropDownListBoxSelectionChange, this));

    drop_down_window_->SetListControl(drop_down_list_box_);
}


void ComboBox::UninitializeDropDownListBox() {

    if (drop_down_list_box_ == nullptr) {
        return;
    }

    drop_down_list_box_->SetMouseMoveCallback(nullptr);
    drop_down_list_box_subscription_.Unsubscribe();
}


void ComboBox::InitializeEditBox() {

    AddChild(edit_box_);
    edit_box_->SetIsVisible(IsEditable());
    edit_box_->SetBorder(Frame(0));
    edit_box_->SetParagraphAlignment(ParagraphAlignment::Center);
    edit_box_->SetIsMultiline(false);
    edit_box_->SetAcceptReturn(false);

    edit_box_subscription_ = edit_box_->TextChangedEvent().Subscribe(
        std::bind(&ComboBox::EditBoxTextChange, this));
}


void ComboBox::UninitializeEditBox() {

    if (edit_box_ == nullptr) {
        return;
    }

    RemoveChild(edit_box_);
    edit_box_subscription_.Unsubscribe();
}


void ComboBox::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    auto edit_box_rect = DetermineTextRect();
    edit_box_->SetRect(edit_box_rect);
}


void ComboBox::UpdateStyle() {

    __super::UpdateStyle();

    SetBackgroundColor([this]() {

        if (!IsEnabledInContext()) {
            return Color::FromRGB(internal::ControlBackgroundColorRGB);
        }

        return Color::FromRGB(internal::ControlContentColorRGB);
    }());

    SetBorderColor([this]() {

        if (IsPressed() || IsMouseOver()) {
            return Color::FromRGB(internal::ButtonActivedBorderColorRGB);
        }

        return Color::Black();
    }());
}


void ComboBox::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    float button_width = DropDownButtonWidth();
    auto button_rect = ContentRect();
    button_rect.position.x = button_rect.position.x + button_rect.size.width - button_width;
    button_rect.size.width = button_width;

    float top_edge_length = button_width / 2;
    if (top_edge_length / 2 > button_rect.size.height / 2) {
        top_edge_length = button_rect.size.height / 2;
    }

    float height = top_edge_length / 2;
    float half_height = height / 2;
    Point center_point(
        button_rect.position.x + button_rect.size.width / 2,
        button_rect.position.y + button_rect.size.height / 2);

    Point left_point(center_point.x - height, center_point.y - half_height);
    Point right_point(center_point.x + height, center_point.y - half_height);
    Point bottom_point(center_point.x, center_point.y + half_height);

    auto path = canvas.CreatePathGeometry();
    if (!path) {
        return;
    }

    auto sink = path.Open();
    if (!sink) {
        return;
    }

    sink.BeginFigure(left_point, GeometrySink::BeginFigureOption::Fill);
    sink.AddLine(right_point);
    sink.AddLine(bottom_point);
    sink.EndFigure(GeometrySink::EndFigureOption::Close);
    sink.Close();

    auto state_guard = canvas.PushState();
    canvas.SetBrushWithColor(DropDownButtonColor());
    canvas.DrawGeometry(path);
}


zaf::Rect ComboBox::DetermineTextRect() {

    auto rect = __super::DetermineTextRect();

    auto deflated_frame = text_inset_;
    deflated_frame.right += DropDownButtonWidth();
    rect.Deflate(deflated_frame);
    return rect;
}


void ComboBox::SetTextInset(const Frame& inset) {

    if (text_inset_ == inset) {
        return;
    }

    text_inset_ = inset;
    NeedRelayout();
}


Color ComboBox::DropDownButtonColor() const {
    return drop_down_button_color_;
}

void ComboBox::SetDropDownButtonColor(const Color& color) {
    drop_down_button_color_ = color;
    NeedRepaint();
}


float ComboBox::DropDownButtonWidth() const {
    return drop_down_button_width_;
}

void ComboBox::SetDropDownButtonWidth(float width) {

    drop_down_button_width_ = width;
    NeedRepaint();
}


std::size_t ComboBox::MinVisibleItemCount() const {
    return min_visible_item_count_;
}

void ComboBox::SetMinVisibleItemCount(std::size_t count) {

    auto max_count = MaxVisibleItemCount();
    auto revised_count = std::min(count, max_count);
    min_visible_item_count_ = revised_count;
}


std::size_t ComboBox::MaxVisibleItemCount() const {
    return max_visible_item_count_;
}


void ComboBox::SetMaxVisibleItemCount(std::size_t count) {

    auto min_count = MinVisibleItemCount();
    auto revised_count = (std::max)(count, min_count);
    max_visible_item_count_ = revised_count;
}


bool ComboBox::IsEditable() const {
    return is_editable_;
}


void ComboBox::SetIsEditable(bool is_editable) {
    
    is_editable_ = is_editable;

    edit_box_->SetText(Text());
    edit_box_->SetIsVisible(is_editable);
}


Observable<ComboBoxSelectionChangedInfo> ComboBox::SelectionChangedEvent() const {
    return selection_changed_event_.GetObservable();
}


void ComboBox::OnSelectionChanged(const ComboBoxSelectionChangedInfo& event_info) {
    selection_changed_event_.Raise(event_info);
}


void ComboBox::SetDropDownListBox(const std::shared_ptr<ComboBoxDropDownListBox>& list_box) {

    if (list_box == drop_down_list_box_) {
        return;
    }

    UninitializeDropDownListBox();

    auto previous_drop_down_list_box = drop_down_list_box_;

    drop_down_list_box_ = list_box == nullptr ? Create<ComboBoxDropDownListBox>() : list_box;
    InitializeDropDownListBox();

    OnDropDownListBoxChanged(previous_drop_down_list_box);
}


void ComboBox::SetEditBox(const std::shared_ptr<ComboBoxEditBox>& edit_box) {

    if (edit_box == edit_box_) {
        return;
    }

    UninitializeEditBox();

    auto previous_edit_box = edit_box_;

    edit_box_ = edit_box == nullptr ? Create<ComboBoxEditBox>() : edit_box;
    InitializeEditBox();

    OnEditBoxChanged(previous_edit_box);
}


void ComboBox::OnClick(const ClickInfo& event_info) {

    __super::OnClick(event_info);

    //Save current selected index in order to recover when the window closed.
    recovered_selected_index_ = drop_down_list_box_->GetFirstSelectedItemIndex();
    need_recover_selected_index_ = true;

    PopupDropDownWindow();
}


void ComboBox::PopupDropDownWindow() {

    auto window = Window();
    if (window == nullptr) {
        return;
    }

    auto window_rect = *RectInWindow();
    window_rect.Inflate(-1, 0);
    window_rect.position.y += Height() - 2;

    std::size_t visible_item_count = drop_down_list_box_->GetItemCount();
    visible_item_count = (std::max)(visible_item_count, MinVisibleItemCount());
    visible_item_count = (std::min)(visible_item_count, MaxVisibleItemCount());

    auto drop_down_list_box_border = drop_down_list_box_->Border();

    window_rect.size.height =
        CalculateDropDownListHeight(visible_item_count) + drop_down_list_box_border.top + drop_down_list_box_border.bottom;

    window_rect = ToPixelAligned(window_rect, window->GetDPI());

    POINT screen_position = FromDIPs(window_rect.position, window->GetDPI()).ToPOINT();
    ClientToScreen(window->Handle(), &screen_position);
    window_rect.position = ToDIPs(Point::FromPOINT(screen_position), window->GetDPI());

    drop_down_window_->SetOwner(window);
    drop_down_window_->SetRect(window_rect);
    drop_down_window_->Show();
}


float ComboBox::CalculateDropDownListHeight(std::size_t visible_item_count) {

    float height = 0;

    auto data_source = drop_down_list_box_->DataSource();
    auto delegate = drop_down_list_box_->Delegate();

    if (data_source && delegate) {

        if (delegate->HasVariableItemHeight()) {

            std::size_t item_count = data_source->GetDataCount();
            for (auto index : Range(0, std::min(item_count, visible_item_count))) {
                auto item_data = data_source->GetDataAtIndex(index);
                height += delegate->EstimateItemHeight(index, item_data);
            }
        }
        else {
            height = delegate->EstimateItemHeight(0, nullptr) * visible_item_count;
        }
    }

    if (height == 0) {
        height = ContentSize().height;
    }

    return height;
}


void ComboBox::DropDownWindowDestroy() {

    if (!need_recover_selected_index_) {
        return;
    }

    //Recover the selected index.
    if (recovered_selected_index_) {
        drop_down_list_box_->SelectItemAtIndex(*recovered_selected_index_);
    }
    else {
        drop_down_list_box_->UnselectAllItems();
    }
}


void ComboBox::DropDownListBoxMouseMove(const Point& position) {

    auto index = drop_down_list_box_->FindItemIndexAtPosition(position);
    if (!index) {
        return;
    }

    auto guard = drop_down_list_box_action_.Set(DropDownListBoxAction::Nothing);
    drop_down_list_box_->SelectItemAtIndex(*index);
}


void ComboBox::OnKeyDown(const KeyDownInfo& event_info) {

    __super::OnKeyDown(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    bool is_handled{};

    auto key = event_info.Message().Key();
    if ((key == Key::Up) || (key == Key::Down)) {

        if (SelectNextDropDownListItem(key == Key::Up)) {
            is_handled = true;
        }
    }
    else if ((key == Key::Enter) || (key == Key::Escape)) {

        ConfirmSelection(key == Key::Escape);
        is_handled = true;
    }
    
    if (is_handled) {
        event_info.MarkAsHandled();
    }
}


bool ComboBox::SelectNextDropDownListItem(bool reverse) {

    auto item_count = drop_down_list_box_->GetItemCount();
    if (item_count == 0) {
        return false;
    }

    auto old_selected_index = drop_down_list_box_->GetFirstSelectedItemIndex();

    std::size_t new_selected_index{};
    if (!old_selected_index) {
        new_selected_index = reverse ? item_count - 1 : 0;
    }
    else {
        
        new_selected_index = *old_selected_index;
        if (reverse) {
            if (new_selected_index > 0) {
                --new_selected_index;
            }
        }
        else {
            if (new_selected_index < item_count - 1) {
                ++new_selected_index;
            }
        }
    }

    auto guard = drop_down_list_box_action_.Set(DropDownListBoxAction::ChangeText);
    drop_down_list_box_->SelectItemAtIndex(new_selected_index);
    drop_down_list_box_->ScrollToItemAtIndex(new_selected_index);
    return true;
}


void ComboBox::DropDownListBoxSelectionChange() {
    
    if (drop_down_list_box_action_.Get() == DropDownListBoxAction::Nothing) {
        return;
    }

    //Current control could be removed from parent and be destroyed during the handling of 
    //selection changed event, "this" would become a dangling pointer in such case. So we keep a
    //shared pointer here to prevent it from being destroyed.
    auto life_holder = shared_from_this();

    //Selection is changed, do not recover previous selected index.
    need_recover_selected_index_ = false;

    auto selected_index = drop_down_list_box_->GetFirstSelectedItemIndex();
    if (selected_index) {

        std::wstring text;

        auto delegate = drop_down_list_box_->Delegate();
        if (delegate) {

            text = delegate->GetItemText(
                *selected_index,
                drop_down_list_box_->GetItemDataAtIndex(*selected_index));
        }

        ChangeSelectionText(text, TextChangeSource::DropDownListBox);
        NotifySelectionChange();
    }

    if (drop_down_list_box_action_.Get() == DropDownListBoxAction::ChangeText) {
        return;
    }

    drop_down_window_->Close();
}


void ComboBox::EditBoxTextChange() {

    if (! IsEditable()) {
        return;
    }

    if (edit_box_action_.Get() == EditBoxAction::Nothing) {
        return;
    }

    ChangeSelectionText(edit_box_->Text(), TextChangeSource::EditBox);
    NotifySelectionChange();
}


void ComboBox::ConfirmSelection(bool discard_drop_down_list_selection) {

    //There is selection in drop down list.
    if (drop_down_list_box_->GetSelectedItemCount() > 0) {

        if (discard_drop_down_list_selection) {
            drop_down_window_->Close();
        }
        else {

            //Simulate changing the selection of drop down list box
            auto guard = drop_down_list_box_action_.Set(DropDownListBoxAction::CloseDropDownWindow);
            DropDownListBoxSelectionChange();
        }
    }
    //There is no selection in drop down list, this typecially occurs when typing on
    //an editable combo box.
    else {

        drop_down_window_->Close();

        if (IsEditable()) {

            //Explicit change the text again in order to trigger the notification.
            ChangeSelectionText(edit_box_->Text(), TextChangeSource::EditBox);
            NotifySelectionChange();
        }
    }
}


void ComboBox::ChangeSelectionText(const std::wstring& text, TextChangeSource source) {

    auto guard = text_change_source_.Set(source);
    SetText(text);
}


void ComboBox::OnTextChanged(const TextChangedInfo& event_info) {

    __super::OnTextChanged(event_info);

    if (text_change_source_.Get() != TextChangeSource::DropDownListBox) {

        //Unselect drop down list item if the change is from other source.
        auto guard = drop_down_list_box_action_.Set(DropDownListBoxAction::Nothing);
        drop_down_list_box_->UnselectAllItems();
    }

    if (text_change_source_.Get() != TextChangeSource::EditBox) {
        if (IsEditable()) {

            auto guard = edit_box_action_.Set(EditBoxAction::Nothing);
            auto text = Text();
            edit_box_->SetText(text);

            //Select all text in edit box if the change is from other source 
            //and edit box is focused.
            if (edit_box_->IsFocused()) {
                edit_box_->SetSelectionRange(Range(0, text.length()));
            }
        }
    }
}


void ComboBox::NotifySelectionChange() {
    OnSelectionChanged(ComboBoxSelectionChangedInfo{ As<ComboBox>(shared_from_this()) });
}


void ComboBox::OnFocusGained(const FocusGainedInfo& event_info) {

    __super::OnFocusGained(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (IsEditable()) {

        edit_box_->SetIsFocused(true);
        event_info.MarkAsHandled();
    }
}


void ComboBoxDropDownListBox::Initialize() {

    __super::Initialize();

    SetDelegate(std::make_shared<DropDownListBoxDelegate>());

    SetAllowHorizontalScroll(false);
    SetAutoHideScrollBars(true);
}


void ComboBoxDropDownListBox::OnMouseMove(const MouseMoveInfo& event_info) {

    __super::OnMouseMove(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (!IsCapturingMouse()) {
        if (mouse_move_callback_) {
            mouse_move_callback_(event_info.PositionAtSender());
        }
    }
}


std::shared_ptr<ListItem> ComboBoxDropDownListBox::DropDownListBoxDelegate::CreateItem(
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    auto result = __super::CreateItem(item_index, item_data);

    result->Subscriptions() += result->StyleUpdateEvent().Subscribe(
        [](const StyleUpdateInfo& event_info) {

        auto item = As<ListItem>(event_info.Source());

        //Drop down list is always inactive, display active selection color instead of inactive
        //selection color.
        item->SetBackgroundColor(
            item->IsSelectedInContext() ?
            Color::FromRGB(internal::ControlSelectedActivedColorRGB) :
            Color::Transparent());
    });

    return result;
}


void ComboBoxEditBox::OnKeyDown(const KeyDownInfo& event_info) {

    //Don't handle these key events, so that they can be handled in combo box.
    auto key = event_info.Message().Key();
    if (key == Key::Up || key == Key::Down || key == Key::Enter || key == Key::Escape) {
        return;
    }

    __super::OnKeyDown(event_info);
}

}