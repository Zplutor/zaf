#include <zaf/control/combo_box.h>
#include <algorithm>
#include <zaf/base/as.h>
#include <zaf/base/container/utility/range.h>
#include <zaf/base/event_utility.h>
#include <zaf/control/button.h>
#include <zaf/control/internal/combo_box_drop_down_window.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include <zaf/control/text_box.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/dpi.h>
#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/internal/theme.h>
#include <zaf/object/type_definition.h>
#include <zaf/serialization/properties.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/window.h>

#ifdef min
#undef min
#endif

namespace zaf {
namespace {

const wchar_t* const kDropDownButtonColorPickerPropertyName = L"DropDownButtonColorPicker";
const wchar_t* const kDropDownButtonWidthPropertyName = L"DropDownButtonWidth";
const wchar_t* const kIsEditablePropertyName = L"IsEditable";
const wchar_t* const kMaxVisibleItemCountPropertyName = L"MaxVisibleItemCount";
const wchar_t* const kMinVisibleItemCountPropertyName = L"MinVisibleItemCount";
const wchar_t* const kSelectionChangeEventPropertyName = L"SelectionChangedEvent";

}


ZAF_DEFINE_TYPE(ComboBox)
ZAF_DEFINE_TYPE_PROPERTY(DropDownButtonColor)
ZAF_DEFINE_TYPE_PROPERTY(DropDownButtonWidth)
ZAF_DEFINE_TYPE_PROPERTY(DropDownListBox)
ZAF_DEFINE_TYPE_PROPERTY(EditTextBox)
ZAF_DEFINE_TYPE_PROPERTY(MinVisibleItemCount)
ZAF_DEFINE_TYPE_PROPERTY(MaxVisibleItemCount)
ZAF_DEFINE_TYPE_END

ZAF_DEFINE_TYPE(ComboBoxDropDownListBox)
ZAF_DEFINE_TYPE_END

ZAF_DEFINE_TYPE(ComboBoxEditTextBox)
ZAF_DEFINE_TYPE_END


ComboBox::ComboBox() : 
    drop_down_list_box_action_(DropDownListBoxAction::CloseDropDownWindow),
    edit_text_box_action_(EditTextBoxAction::ChangeText),
    text_change_source_(TextChangeSource::None) {

}


ComboBox::~ComboBox() {

    UninitializeDropDownListBox();
    UninitializeEditTextBox();

    if (drop_down_window_) {
        drop_down_window_->Close();
    }
}


void ComboBox::Initialize() {

    __super::Initialize();

    SetBorder(Frame{ 1 });
    SetTextInset(Frame{ 3, 1, 1, 1 });

    SetBackgroundColorPicker([](const Control& control) {

        if (! control.IsEnabled()) {
            return Color::FromRGB(internal::ControlBackgroundColorRGB);
        }

        return Color::FromRGB(internal::ControlContentColorRGB);
    });

    SetBorderColorPicker([](const Control& control) {

        const auto& combo_box = dynamic_cast<const ComboBox&>(control);

        if (combo_box.IsPressed() || combo_box.IsMouseOver()) {
            return Color::FromRGB(internal::ButtonActivedBorderColorRGB);
        }

        return Color::Black();
    });

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

    edit_text_box_ = Create<ComboBoxEditTextBox>();
    InitializeEditTextBox();
}


void ComboBox::InitializeDropDownListBox() {

    drop_down_list_box_->SetMouseMoveCallback(
        std::bind(&ComboBox::DropDownListBoxMouseMove, this, std::placeholders::_1));

    drop_down_list_box_subscription_ = drop_down_list_box_->SelectionChangeEvent().Subscribe(
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


void ComboBox::InitializeEditTextBox() {

    AddChild(edit_text_box_);
    edit_text_box_->SetIsVisible(IsEditable());
    edit_text_box_->SetBorder(Frame(0));
    edit_text_box_->SetParagraphAlignment(ParagraphAlignment::Center);
    edit_text_box_->SetIsMultiline(false);
    edit_text_box_->SetAcceptReturn(false);

    edit_text_box_subscription_ = edit_text_box_->TextChangedEvent().Subscribe(
        std::bind(&ComboBox::EditTextBoxTextChange, this));
}


void ComboBox::UninitializeEditTextBox() {

    if (edit_text_box_ == nullptr) {
        return;
    }

    RemoveChild(edit_text_box_);
    edit_text_box_subscription_.Unsubscribe();
}


void ComboBox::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    auto text_box_rect = GetTextRect();
    edit_text_box_->SetRect(text_box_rect);
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
    if (path == nullptr) {
        return;
    }

    auto sink = path.Open();
    if (sink == nullptr) {
        return;
    }

    sink.BeginFigure(left_point, GeometrySink::BeginFigureOption::Fill);
    sink.AddLine(right_point);
    sink.AddLine(bottom_point);
    sink.EndFigure(GeometrySink::EndFigureOption::Close);
    sink.Close();

    Canvas::StateGuard state_guard(canvas);
    canvas.SetBrushWithColor(DropDownButtonColor());
    canvas.DrawGeometry(path);
}


zaf::Rect ComboBox::GetTextRect() {

    auto rect = __super::GetTextRect();

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


ColorPicker ComboBox::DropDownButtonColorPicker() const {

    auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(kDropDownButtonColorPickerPropertyName);
    if (color_picker != nullptr) {
        return *color_picker;
    }
    else {
        return [](const Control&) { return Color::Black(); };
    }
}

void ComboBox::SetDropDownButtonColorPicker(const ColorPicker& color_picker) {

    GetPropertyMap().SetProperty(kDropDownButtonColorPickerPropertyName, color_picker);
    NeedRepaint();
}


float ComboBox::DropDownButtonWidth() const {

    auto width = GetPropertyMap().TryGetProperty<float>(kDropDownButtonWidthPropertyName);
    if (width != nullptr) {
        return *width;
    }
    else {
        return 12;
    }
}

void ComboBox::SetDropDownButtonWidth(float width) {

    GetPropertyMap().SetProperty(kDropDownButtonWidthPropertyName, width);
    NeedRepaint();
}


std::size_t ComboBox::MinVisibleItemCount() const {

    auto count = GetPropertyMap().TryGetProperty<std::size_t>(kMinVisibleItemCountPropertyName);
    if (count != nullptr) {
        return *count;
    }
    else {
        return 1;
    }
}

void ComboBox::SetMinVisibleItemCount(std::size_t count) {

    auto max_count = MaxVisibleItemCount();
    auto revised_count = std::min(count, max_count);
    GetPropertyMap().SetProperty(kMinVisibleItemCountPropertyName, revised_count);
}


std::size_t ComboBox::MaxVisibleItemCount() const {

    auto count = GetPropertyMap().TryGetProperty<std::size_t>(kMaxVisibleItemCountPropertyName);
    if (count != nullptr) {
        return *count;
    }
    else {
        return std::numeric_limits<std::size_t>::max();
    }
}


void ComboBox::SetMaxVisibleItemCount(std::size_t count) {

    auto min_count = MinVisibleItemCount();
    auto revised_count = std::max(count, min_count);
    GetPropertyMap().SetProperty(kMaxVisibleItemCountPropertyName, revised_count);
}


bool ComboBox::IsEditable() const {
    
    auto is_editable = GetPropertyMap().TryGetProperty<bool>(kIsEditablePropertyName);
    if (is_editable != nullptr) {
        return *is_editable;
    }
    else {
        return false;
    }
}


void ComboBox::SetIsEditable(bool is_editable) {
    
    GetPropertyMap().SetProperty(kIsEditablePropertyName, is_editable);

    edit_text_box_->SetText(Text());
    edit_text_box_->SetIsVisible(is_editable);
}


Observable<ComboBoxSelectionChangedInfo> ComboBox::SelectionChangedEvent() {

    return GetEventObservable<ComboBoxSelectionChangedInfo>(
        GetPropertyMap(),
        kSelectionChangeEventPropertyName);
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


void ComboBox::SetEditTextBox(const std::shared_ptr<ComboBoxEditTextBox>& text_box) {

    if (text_box == edit_text_box_) {
        return;
    }

    UninitializeEditTextBox();

    auto previous_edit_text_box = edit_text_box_;

    edit_text_box_ = text_box == nullptr ? Create<ComboBoxEditTextBox>() : text_box;
    InitializeEditTextBox();

    OnEditTextBoxChanged(previous_edit_text_box);
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

    auto window_rect = AbsoluteRect();
    window_rect.Inflate(-1, 0);
    window_rect.position.y += Height() - 2;

    std::size_t visible_item_count = drop_down_list_box_->GetItemCount();
    visible_item_count = std::max(visible_item_count, MinVisibleItemCount());
    visible_item_count = std::min(visible_item_count, MaxVisibleItemCount());

    auto drop_down_list_box_border = drop_down_list_box_->Border();

    window_rect.size.height =
        CalculateDropDownListHeight(visible_item_count) + drop_down_list_box_border.top + drop_down_list_box_border.bottom;

    window_rect = Align(window_rect);

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
            for (auto index : Range(std::min(item_count, visible_item_count))) {
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

    auto key = event_info.Message().VirtualKey();
    if ((key == VK_UP) || (key == VK_DOWN)) {

        if (SelectNextDropDownListItem(key == VK_UP)) {
            is_handled = true;
        }
    }
    else if ((key == VK_RETURN) || (key == VK_ESCAPE)) {

        ConfirmSelection(key == VK_ESCAPE);
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


void ComboBox::EditTextBoxTextChange() {

    if (! IsEditable()) {
        return;
    }

    if (edit_text_box_action_.Get() == EditTextBoxAction::Nothing) {
        return;
    }

    ChangeSelectionText(edit_text_box_->Text(), TextChangeSource::EditTextBox);
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
            ChangeSelectionText(edit_text_box_->Text(), TextChangeSource::EditTextBox);
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

    if (text_change_source_.Get() != TextChangeSource::EditTextBox) {
        if (IsEditable()) {

            auto guard = edit_text_box_action_.Set(EditTextBoxAction::Nothing);
            auto text = Text();
            edit_text_box_->SetText(text);

            //Select all text in edit text box if the change is from other source 
            //and edit text box is focused.
            if (edit_text_box_->IsFocused()) {
                edit_text_box_->SetSelectionRange(TextRange(0, text.length()));
            }
        }
    }
}


void ComboBox::NotifySelectionChange() {
    OnSelectionChanged(ComboBoxSelectionChangedInfo{ As<ComboBox>(shared_from_this()) });
}


void ComboBox::OnSelectionChanged(const ComboBoxSelectionChangedInfo& event_info) {

    auto observer = GetEventObserver<ComboBoxSelectionChangedInfo>(
        GetPropertyMap(),
        kSelectionChangeEventPropertyName);

    if (observer) {
        observer->OnNext(event_info);
    }
}


void ComboBox::OnFocusGained(const FocusGainedInfo& event_info) {

    __super::OnFocusGained(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (IsEditable()) {

        edit_text_box_->SetIsFocused(true);
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

    result->SetBackgroundColorPicker([](const Control& control) {

        //Drop down list is always inactive, display active selection color instead of inactive
        //selection color.
        return 
            control.IsSelected() ? 
            Color::FromRGB(internal::ControlSelectedActivedColorRGB) :
            Color::Transparent();
    });

    return result;
}


void ComboBoxEditTextBox::OnKeyDown(const KeyDownInfo& event_info) {

    //Don't handle these key events, so that they can be handled in combo box.
    auto key = event_info.Message().VirtualKey();
    if (key == VK_UP || key == VK_DOWN || key == VK_RETURN || key == VK_ESCAPE) {
        return;
    }

    __super::OnKeyDown(event_info);
}

}