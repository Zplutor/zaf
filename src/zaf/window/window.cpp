#include <zaf/window/window.h>
#include <Windowsx.h>
#include <zaf/application.h>
#include <zaf/base/error/system_error.h>
#include <zaf/base/event_utility.h>
#include <zaf/creation.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/dpi.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/internal/tab_stop_utility.h>
#include <zaf/internal/theme.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_node_parse_helper.h>
#include <zaf/object/type_definition.h>
#include <zaf/serialization/properties.h>
#include <zaf/window/inspector/inspector_window.h>
#include <zaf/window/tooltip_window.h>
#include <zaf/window/message/hit_test_message.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>
#include <zaf/window/window_class_registry.h>
#include <zaf/window/window_holder.h>

namespace zaf {
namespace {

constexpr const wchar_t* const kActivateOptionPropertyName = L"ActivateOption";
constexpr const wchar_t* const kCanMaximizePropertyName = L"CanMaximize";
constexpr const wchar_t* const kCanMinimizePropertyName = L"CanMinimize";
constexpr const wchar_t* const ClosingEventPropertyName = L"ClosingEvent";
constexpr const wchar_t* const DestroyedEventPropertyName = L"DestroyedEvent";
constexpr const wchar_t* const kHasBorderPropertyName = L"HasBorder";
constexpr const wchar_t* const kHasSystemMenuPropertyName = L"HasSystemMenu";
constexpr const wchar_t* const kHasTitleBarPropertyName = L"HasTitleBar";
constexpr const wchar_t* const kInitialRectStylePropertyName = L"InitialRectStyle";
constexpr const wchar_t* const kIsPopupPropertyName = L"IsPopup";
constexpr const wchar_t* const kIsSizablePropertyName = L"IsSizable";
constexpr const wchar_t* const kIsToolWindowPropertyName = L"IsToolWindow";
constexpr const wchar_t* const kIsTopmostPropertyName = L"IsTopmost";
constexpr const wchar_t* const kOwnerPropertyName = L"Owner";
constexpr const wchar_t* const MessageReceivedEventPropertyName = L"MessageReceivedEvent";
constexpr const wchar_t* const MessageHandledEventPropertyName = L"MessageHandledEvent";
constexpr const wchar_t* const MouseCaptureControlChangedEventPropertyName = 
    L"MouseCaptureControlChangedEvent";
constexpr const wchar_t* const kTitlePropertyName = L"Title";
constexpr const wchar_t* const HandleCreatedEventPropertyName = L"HandleCreatedEvent";
constexpr const wchar_t* const RootControlChangedEventPropertyName = L"RootControlChangedEvent";
constexpr const wchar_t* const ShowEventPropertyName = L"ShowEvent";
constexpr const wchar_t* const HideEventPropertyName = L"HideEvent";
constexpr const wchar_t* const FocusGainedEventPropertyName = L"FocusGainedEvent";
constexpr const wchar_t* const FocusLostEventPropertyName = L"FocusLostEvent";


Point TranslateAbsolutePositionToControlPosition(
    const Point& absolute_position, 
    const Control& control ) {

    zaf::Rect control_absolute_rect = control.AbsoluteRect();

    Point result;
    result.x = absolute_position.x - control_absolute_rect.position.x;
    result.y = absolute_position.y - control_absolute_rect.position.y;
    return result;
}


template<typename E>
bool RouteKeyboardEventGeneric(
    const std::shared_ptr<Control>& source, 
    const Message& message,
    void (Control::*event_function)(const E&)) {

    if (!source) {
        return false;
    }

    auto event_info_state = std::make_shared<internal::KeyboardEventSharedState>(source, message);

    for (auto sender = source; sender; sender = sender->Parent()) {

        E event_info{ event_info_state, sender };
        (sender.get()->*event_function)(event_info);
    }

    return event_info_state->IsHandled();
}


template<typename E>
bool RouteMouseEventGeneric(
    const std::shared_ptr<Control>& source,
    const MouseMessage& message,
    void (Control::*event_function)(const E&)) {

    if (!source) {
        return false;
    }

    auto position_at_source = TranslateAbsolutePositionToControlPosition(
        message.MousePosition(), 
        *source);

    auto event_info_state = std::make_shared<internal::MouseEventSharedState>(
        source, 
        message.Inner(), 
        position_at_source);

    auto sender = source;
    auto position_at_sender = position_at_source;
    while (sender) {

        E event_info{ event_info_state, sender, position_at_sender };
        (sender.get()->*event_function)(event_info);

        position_at_sender = sender->TranslateToParentPoint(position_at_sender);
        sender = sender->Parent();
    }

    return event_info_state->IsHandled();
}


template<typename E>
void RouteFocusEventGeneric(
    const std::shared_ptr<Control>& source,
    const std::shared_ptr<Control>& chaning_control,
    void (Control::*event_function)(const E&)) {

    auto event_info_state = std::make_shared<internal::FocusEventSharedState>(
        source, 
        chaning_control);

    bool original_is_focused = source->IsFocused();

    for (auto sender = source; sender; sender = sender->Parent()) {

        E event_info{ event_info_state, sender };
        (sender.get()->*event_function)(event_info);

        //Stop routing event if focused control is changed during the routing.
        if (source->IsFocused() != original_is_focused) {
            break;
        }
    }

}


zaf::Rect ToAlignedPixelsRect(const zaf::Rect& rect, float dpi) {
    return Align(FromDIPs(rect, dpi));
}


class WindowParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) override {
        //Nothing to do.
    }

    void ParseFromNode(const XamlNode& node, Object& reflection_object) override {

        auto& window = dynamic_cast<Window&>(reflection_object);
        ParseControls(node, window);
    }

private:
    void ParseControls(const XamlNode& node, Window& window) {

        std::vector<std::shared_ptr<Control>> controls;

        for (const auto& each_node : node.GetContentNodes()) {

            auto control = internal::CreateObjectFromNode<Control>(each_node);
            if (control == nullptr) {
                continue;
            }

            controls.push_back(control);
        }

        if (!controls.empty()) {
            window.RootControl()->AddChildren(controls);
        }
    }
};

}

ZAF_DEFINE_TYPE(Window)
ZAF_DEFINE_TYPE_PARSER(WindowParser)
ZAF_DEFINE_TYPE_PROPERTY(Owner)
ZAF_DEFINE_TYPE_PROPERTY(InitialRectStyle)
ZAF_DEFINE_TYPE_PROPERTY(Rect)
ZAF_DEFINE_TYPE_PROPERTY(Position)
ZAF_DEFINE_TYPE_PROPERTY(Size)
ZAF_DEFINE_TYPE_PROPERTY(ContentRect)
ZAF_DEFINE_TYPE_PROPERTY(ContentSize)
ZAF_DEFINE_TYPE_PROPERTY(ContentWidth)
ZAF_DEFINE_TYPE_PROPERTY(ContentHeight)
ZAF_DEFINE_TYPE_PROPERTY(MinSize)
ZAF_DEFINE_TYPE_PROPERTY(MaxSize)
ZAF_DEFINE_TYPE_PROPERTY(Width)
ZAF_DEFINE_TYPE_PROPERTY(MinWidth)
ZAF_DEFINE_TYPE_PROPERTY(MaxWidth)
ZAF_DEFINE_TYPE_PROPERTY(Height)
ZAF_DEFINE_TYPE_PROPERTY(MinHeight)
ZAF_DEFINE_TYPE_PROPERTY(MaxHeight)
ZAF_DEFINE_TYPE_PROPERTY(ActivateOption)
ZAF_DEFINE_TYPE_PROPERTY(IsPopup)
ZAF_DEFINE_TYPE_PROPERTY(HasBorder)
ZAF_DEFINE_TYPE_PROPERTY(HasTitleBar)
ZAF_DEFINE_TYPE_PROPERTY(IsSizable)
ZAF_DEFINE_TYPE_PROPERTY(HasSystemMenu)
ZAF_DEFINE_TYPE_PROPERTY(CanMinimize)
ZAF_DEFINE_TYPE_PROPERTY(CanMaximize)
ZAF_DEFINE_TYPE_PROPERTY(IsToolWindow)
ZAF_DEFINE_TYPE_PROPERTY(IsTopmost)
ZAF_DEFINE_TYPE_PROPERTY(Title)
ZAF_DEFINE_TYPE_PROPERTY_DYNAMIC(RootControl)
ZAF_DEFINE_TYPE_PROPERTY(MouseCaptureControl)
ZAF_DEFINE_TYPE_PROPERTY(MouseOverControl)
ZAF_DEFINE_TYPE_PROPERTY(FocusedControl)
ZAF_DEFINE_TYPE_PROPERTY(IsVisible)
ZAF_DEFINE_TYPE_END


LRESULT CALLBACK Window::WindowProcedure(HWND hwnd, UINT message_id, WPARAM wparam, LPARAM lparam) {

    if (message_id == WM_NCCREATE) {
        auto create_struct = reinterpret_cast<const CREATESTRUCTA*>(lparam);
        SetWindowLongPtr(
            hwnd, 
            GWLP_USERDATA, 
            reinterpret_cast<ULONG_PTR>(create_struct->lpCreateParams));
    }

    auto window = GetWindowFromHandle(hwnd);
    if (window) {
        return window->RouteWindowMessage(hwnd, message_id, wparam, lparam);
    }

    return CallWindowProc(DefWindowProc, hwnd, message_id, wparam, lparam);
}


Window::Window() : class_(WindowClassRegistry::Instance().GetDefaultWindowClass()) {

}


Window::Window(const std::wstring& window_class_name) {
    class_ = WindowClassRegistry::Instance().GetOrRegisterWindowClass(window_class_name, nullptr);
}


Window::Window(const std::shared_ptr<WindowClass>& window_class) : class_(window_class) {
    ZAF_EXPECT(class_);
}


Window::~Window() {
    Destroy();
}


void Window::Initialize() {

    __super::Initialize();

    InitializeRootControl(Create<Control>());
}


void Window::CreateWindowHandle() {

    //Revise HasTitleBar property first.
    ReviseHasTitleBar();

    auto owner = Owner();

    DWORD style = 0;
    DWORD extra_style = 0;
    GetHandleStyles(style, extra_style);

    handle_ = CreateWindowEx(
        extra_style,
        reinterpret_cast<LPCWSTR>(class_->GetAtom()),
        Title().c_str(),
        style,
        0,
        0,
        0,
        0,
        owner == nullptr ? nullptr : owner->Handle(),
        nullptr,
        nullptr,
        this);

    auto dpi = static_cast<float>(GetDpiForWindow(handle_));

    RECT window_rect{};
    GetWindowRect(handle_, &window_rect);
    rect_ = ToDIPs(Rect::FromRECT(window_rect), dpi);

    RECT client_rect{};
    GetClientRect(handle_, &client_rect);
    root_control_->SetRect(ToDIPs(Rect::FromRECT(client_rect), dpi));

    CreateRenderer();

    OnHandleCreated(HandleCreatedInfo{ shared_from_this() });
}


void Window::OnHandleCreated(const HandleCreatedInfo& event_info) {

    auto observer = GetEventObserver<HandleCreatedInfo>(
        GetPropertyMap(), 
        HandleCreatedEventPropertyName);

    if (observer) {
        observer->OnNext(event_info);
    }
}


Observable<HandleCreatedInfo> Window::HandleCreatedEvent() {
    return GetEventObservable<HandleCreatedInfo>(GetPropertyMap(), HandleCreatedEventPropertyName);
}


LRESULT Window::HandleWMCREATE(const Message& message) {

    auto dpi = static_cast<float>(GetDpiForWindow(message.WindowHandle()));
    auto initial_rect = GetInitialRect(dpi);
    auto rect_in_pixels = ToAlignedPixelsRect(initial_rect, dpi);

    SetWindowPos(
        message.WindowHandle(),
        nullptr,
        static_cast<int>(rect_in_pixels.position.x),
        static_cast<int>(rect_in_pixels.position.y),
        static_cast<int>(rect_in_pixels.size.width),
        static_cast<int>(rect_in_pixels.size.height),
        SWP_NOZORDER | SWP_NOACTIVATE);

    return 0;
}


Rect Window::GetInitialRect(float dpi) const {

    auto initial_rect_style = InitialRectStyle();
    if (initial_rect_style == InitialRectStyle::Custom) {
        return rect_;
    }

    auto owner = Owner();

    if ((initial_rect_style == InitialRectStyle::CenterInOwner) && 
        (owner != nullptr)) {

        auto owner_rect = owner->Rect();
        Point position(
            owner_rect.position.x + (owner_rect.size.width - rect_.size.width) / 2,
            owner_rect.position.y + (owner_rect.size.height - rect_.size.height) / 2);

        return zaf::Rect(position, rect_.size);
    }

    //TODO: Get current monitor's size.
    float screen_width = ToDIPs(static_cast<float>(GetSystemMetrics(SM_CXSCREEN)), dpi);
    float screen_height = ToDIPs(static_cast<float>(GetSystemMetrics(SM_CYSCREEN)), dpi);
    Point position(
        (screen_width - rect_.size.width) / 2,
        (screen_height - rect_.size.height) / 2);

    return zaf::Rect(position, rect_.size);
}


void Window::CreateRenderer() {

    renderer_ = GraphicFactory::Instance().CreateWindowRenderer(handle_);
}


void Window::RecreateRenderer() {

    root_control_->ReleaseRendererResources();
    CreateRenderer();
}


std::shared_ptr<WindowHolder> Window::CreateHandle() {
    return CreateWindowHandleIfNeeded();
}


std::shared_ptr<WindowHolder> Window::CreateWindowHandleIfNeeded() {

    if (Handle()) {

        auto holder = holder_.lock();
        ZAF_EXPECT(holder);
        return holder;
    }

    CreateWindowHandle();

    auto holder = holder_.lock();
    ZAF_EXPECT(!holder);

    holder = std::make_shared<WindowHolder>(shared_from_this());
    holder_ = holder;
    return holder;
}


void Window::GetHandleStyles(DWORD& handle_style, DWORD& handle_extra_style) const {

    handle_style |= IsPopup() ? WS_POPUP : WS_OVERLAPPED;

    if (HasBorder()) {
        
        handle_style |= WS_BORDER;

        if (HasTitleBar()) {
            handle_style |= WS_CAPTION;
        }

        if (IsSizable()) {
            handle_style |= WS_SIZEBOX;
        }
    }

    if (HasSystemMenu()) {
        handle_style |= WS_SYSMENU;
    }

    if (CanMinimize()) {
        handle_style |= WS_MINIMIZEBOX;
    }

    if (CanMaximize()) {
        handle_style |= WS_MAXIMIZEBOX;
    }

    if (IsToolWindow()) {
        handle_extra_style |= WS_EX_TOOLWINDOW;
    }

    if (IsTopmost()) {
        handle_extra_style |= WS_EX_TOPMOST;
    }

    auto activate_option = ActivateOption();
    if ((activate_option & ActivateOption::NoActivate) == ActivateOption::NoActivate) {
        handle_extra_style |= WS_EX_NOACTIVATE;
    }
}


bool Window::PreprocessMessage(const KeyMessage& message) {

    if (TryToPreprocessTabKeyMessage(message)) {
        return true;
    }

    if (TryToPreprocessInspectorShortcutMessage(message)) {
        return true;
    }

    return false;
}


bool Window::TryToPreprocessTabKeyMessage(const KeyMessage& message) {

    if (message.ID() != WM_KEYDOWN || message.VirtualKey() != VK_TAB) {
        return false;
    }

    if ((focused_control_) && focused_control_->AcceptKeyMessage(message)) {
        return false;
    }

    bool backward = (GetKeyState(VK_SHIFT) >> 15) != 0;
    SwitchFocusedControlByTabKey(backward);
    return true;
}


void Window::SwitchFocusedControlByTabKey(bool backward) {

    auto current_focused_control = focused_control_;
    if (current_focused_control == nullptr) {
        current_focused_control = root_control_;
    }

    auto next_focused_control = internal::FindNextTabStopControl(current_focused_control, backward);
    if (next_focused_control != nullptr) {
        SetFocusedControl(next_focused_control);
    }
}


bool Window::TryToPreprocessInspectorShortcutMessage(const KeyMessage& message) {
#ifndef NDEBUG
    if (message.ID() != WM_KEYDOWN) {
        return false;
    }

    if (message.VirtualKey() != VK_F12) {
        return false;
    }

    if ((GetKeyState(VK_CONTROL) >> 15) == 0) {
        return false;
    }

    ShowInspectorWindow();
    return true;
#else
    return false;
#endif
}


LRESULT Window::RouteWindowMessage(HWND hwnd, UINT id, WPARAM wparam, LPARAM lparam) {

    Message message{ hwnd, id, wparam, lparam };

    //Route message received event first.
    MessageReceivedInfo message_received_info{ shared_from_this(), message };
    OnMessageReceived(message_received_info);

    //Check if the message has been handled, pass it to default window procedure if not.
    auto handle_result = message_received_info.HandleResult();
    if (!handle_result) {
        handle_result = CallWindowProc(DefWindowProc, hwnd, id, wparam, lparam);
    }

    //Route message handled event then.
    MessageHandledInfo message_handled_info{ shared_from_this(), message, *handle_result };
    OnMessageHandled(message_handled_info);

    return *handle_result;
}


void Window::OnMessageReceived(const MessageReceivedInfo& event_info) {

    RaiseMessageReceivedEvent(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    auto result = HandleMessage(event_info.Message());
    if (result) {
        event_info.MarkAsHandled(*result);
    }
}


void Window::RaiseMessageReceivedEvent(const MessageReceivedInfo& event_info) {

    if (event_info.Message().ID() == WM_NCDESTROY) {
        return;
    }

    auto event_observer = GetEventObserver<MessageReceivedInfo>(
        GetPropertyMap(),
        MessageReceivedEventPropertyName);

    if (event_observer) {
        event_observer->OnNext(event_info);
    }
}


std::optional<LRESULT> Window::HandleMessage(const Message& message) {

    switch (message.ID()) {
    case WM_CREATE:
        return HandleWMCREATE(message);

    case WM_NCCALCSIZE:
        return HandleWMNCCALCSIZE(message);

    case WM_ERASEBKGND:
        //Don't erase background to avoid blinking.
        return TRUE;
        
    case WM_PAINT:
        HandleWMPAINT();
        return 0;

    case WM_GETMINMAXINFO: {
        auto min_max_info = reinterpret_cast<MINMAXINFO*>(message.LParam());
        min_max_info->ptMinTrackSize.x = static_cast<LONG>(MinWidth());
        min_max_info->ptMinTrackSize.y = static_cast<LONG>(MinHeight());
        min_max_info->ptMaxTrackSize.x = static_cast<LONG>(MaxWidth());
        min_max_info->ptMaxTrackSize.y = static_cast<LONG>(MaxHeight());
        return 0;
    }

    case WM_SHOWWINDOW:
        HandleWMSHOWWINDOW(ShowWindowMessage{ message });
        return 0;

    case WM_SIZE:
        HandleSizeMessage(message);
        return 0;

    case WM_MOVE:
        HandleMoveMessage();
        return 0;

    case WM_SETFOCUS:
        HandleWMSETFOCUS(SetFocusMessage{ message });
        return 0;

    case WM_KILLFOCUS: 
        HandleWMKILLFOCUS(KillFocusMessage{ message });
        return 0;

    case WM_MOUSEACTIVATE: {
        auto activate_option = ActivateOption();
        bool no_activate = (activate_option & ActivateOption::NoActivate) == ActivateOption::NoActivate;
        bool discard_message = (activate_option & ActivateOption::DiscardMouseMessage) == ActivateOption::DiscardMouseMessage;
        if (no_activate) {
            return discard_message ? MA_NOACTIVATEANDEAT : MA_NOACTIVATE;
        }
        else {
            return discard_message ? MA_ACTIVATEANDEAT : MA_ACTIVATE;
        }
    }

    case WM_CAPTURECHANGED: {
        CancelMouseCapture();
        return 0;
    }

    case WM_NCHITTEST: {
        auto hit_test_result = HitTest(HitTestMessage{ message });
        if (hit_test_result) {
            return static_cast<LRESULT>(*hit_test_result);
        }
        else {
            return std::nullopt;
        }
    }

    case WM_SETCURSOR: {
        bool is_changed = ChangeMouseCursor(message);
        if (is_changed) {
            return TRUE;
        }
        else {
            return std::nullopt;
        }
    }

    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL: {
        //Mouse wheel messages are not sent to an unfocused window even if it captures
        //the mouse, because these messages are only sent to focused window.
        //But we wish these messages have the same behaviour as other mouse input messages,
        //so the messages are redircted to the window which is capturing the mouse.
        if (RedirectMouseWheelMessage(message)) {
            return 0;
        }

        if (HandleMouseMessage(MouseWheelMessage{ message })) {
            return 0;
        }

        return std::nullopt;
    }

    case WM_MOUSEMOVE:
    case WM_NCMOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_NCLBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP: {
        if (HandleMouseMessage(MouseMessage{ message })) {
            return 0;
        }
        return std::nullopt;
    }

    case WM_MOUSEHOVER:
    case WM_NCMOUSEHOVER:
        OnMouseHover(message);
        return 0;

    case WM_MOUSELEAVE:
    case WM_NCMOUSELEAVE:
        OnMouseLeave(message);
        return 0;

    case WM_KEYDOWN: 
        if (OnKeyDown(message)) {
            return 0;
        }
        return std::nullopt;

    case WM_KEYUP:
        if (OnKeyUp(message)) {
            return 0;
        }
        return std::nullopt;

    case WM_CHAR:
        if (OnCharInput(message)) {
            return 0;
        }
        return std::nullopt;

    case WM_CLOSE:
        if (!HandleWMCLOSE()) {
            return 0;
        }
        return std::nullopt;

    case WM_DESTROY:
        HandleWMDESTROY();
        return 0;

    case WM_NCDESTROY:
        HandleWMNCDESTROY();
        return 0;

    default:
        return std::nullopt;
    }
}


void Window::OnMessageHandled(const MessageHandledInfo& event_info) {

    auto observer = GetEventObserver<MessageHandledInfo>(
        GetPropertyMap(), 
        MessageHandledEventPropertyName);

    if (observer) {
        observer->OnNext(event_info);
    }
}


void Window::HandleWMPAINT() {

    zaf::Rect dirty_rect;

    RECT win32_rect{};
    if (GetUpdateRect(handle_, &win32_rect, TRUE)) {
        dirty_rect = ToDIPs(Rect::FromRECT(win32_rect), GetDPI());
    }
    else {
        dirty_rect = root_control_->Rect();
    }

    //The update rect must be validated before painting.
    //Because some controls may call NeedRepaint while it is painting,
    //and this may fails if there is an invalidated update rect.
    ValidateRect(handle_, nullptr);

    renderer_.BeginDraw();

    Canvas canvas(renderer_, root_control_->Rect(), dirty_rect);

    //Paint window background color first.
    {
        Canvas::StateGuard state_guard(canvas);
        canvas.SetBrushWithColor(Color::FromRGB(internal::ControlBackgroundColorRGB));
        canvas.DrawRectangle(dirty_rect);
    }

    root_control_->Repaint(canvas, dirty_rect);

    PaintInspectedControl(canvas, dirty_rect);

    try {
        renderer_.EndDraw();
    }
    catch (const Error& error) {
        if (error.Code() == MakeCOMErrorCode(D2DERR_RECREATE_TARGET)) {
            RecreateRenderer();
        }
    }
}


void Window::PaintInspectedControl(Canvas& canvas, const zaf::Rect& dirty_rect) {

    if (!highlight_control_) {
        return;
    }

    auto control_rect = highlight_control_->AbsoluteRect();
    if (!control_rect.HasIntersection(dirty_rect)) {
        return;
    }

    auto padding_rect = control_rect;
    padding_rect.Deflate(highlight_control_->Border());

    auto content_rect = padding_rect;
    content_rect.Deflate(highlight_control_->Padding());

    auto margin_rect = control_rect;
    margin_rect.Inflate(highlight_control_->Margin());

    auto draw_frame = [&canvas](
        const zaf::Rect& rect,
        const zaf::Rect excluded_rect,
        std::uint32_t color_rgb) {
    
        auto rect_geometry = canvas.CreateRectangleGeometry(rect);
        auto excluded_geometry = canvas.CreateRectangleGeometry(excluded_rect);

        auto frame_geometry = canvas.CreatePathGeometry();
        auto sink = frame_geometry.Open();
        Geometry::Combine(rect_geometry, excluded_geometry, Geometry::CombineMode::Exclude, sink);
        sink.Close();
        
        auto color = Color::FromRGB(color_rgb);
        color.a /= 2.f;
        canvas.SetBrushWithColor(color);
        canvas.DrawGeometry(frame_geometry);
    };

    Canvas::StateGuard state_guard(canvas);
    canvas.PushClippingRect(dirty_rect);

    //Draw content rect.
    draw_frame(content_rect, zaf::Rect{}, internal::InspectedControlContentColor);

    //Draw padding rect.
    draw_frame(padding_rect, content_rect, internal::InspectedControlPaddingColor);

    //Draw border rect.
    draw_frame(control_rect, padding_rect, internal::InspectedControlBorderColor);

    //Draw margin rect.
    draw_frame(margin_rect, control_rect, internal::InspectedControlMarginColor);

    canvas.PopClippingRect();
}


void Window::NeedRepaintRect(const zaf::Rect& rect) {

    if (handle_ != nullptr) {
        RECT win32_rect = ToAlignedPixelsRect(rect, GetDPI()).ToRECT();
        InvalidateRect(handle_, &win32_rect, FALSE);
    }
}


void Window::HandleWMSHOWWINDOW(const ShowWindowMessage& message) {

    internal::ShowWindowEventInfo event_info{ shared_from_this(), message };

    if (message.IsHidden()) {
        OnHide(event_info);
    }
    else {
        OnShow(event_info);
    }
}


void Window::OnShow(const ShowInfo& event_info) {

    auto observer = GetEventObserver<ShowInfo>(GetPropertyMap(), ShowEventPropertyName);
    if (observer) {
        observer->OnNext(event_info);
    }
}


Observable<ShowInfo> Window::ShowEvent() {
    return GetEventObservable<ShowInfo>(GetPropertyMap(), ShowEventPropertyName);
}


void Window::OnHide(const HideInfo& event_info) {

    auto observer = GetEventObserver<HideInfo>(GetPropertyMap(), HideEventPropertyName);
    if (observer) {
        observer->OnNext(event_info);
    }
}


Observable<HideInfo> Window::HideEvent() {
    return GetEventObservable<HideInfo>(GetPropertyMap(), HideEventPropertyName);
}


void Window::HandleWMSETFOCUS(const SetFocusMessage& message) {

    if (auto last_focused_control = last_focused_control_.lock()) {
        last_focused_control_.reset();
        SetFocusedControl(last_focused_control);
    }

    OnFocusGained(WindowFocusGainedInfo{ shared_from_this(), message.Inner() });
}


void Window::OnFocusGained(const WindowFocusGainedInfo& event_info) {

    auto observer = GetEventObserver<WindowFocusGainedInfo>(
        GetPropertyMap(), 
        FocusGainedEventPropertyName);

    if (observer) {
        observer->OnNext(event_info);
    }
}


Observable<WindowFocusGainedInfo> Window::FocusGainedEvent() {
    return GetEventObservable<WindowFocusGainedInfo>(
        GetPropertyMap(), 
        FocusGainedEventPropertyName);
}


void Window::HandleWMKILLFOCUS(const KillFocusMessage& message) {

    last_focused_control_ = FocusedControl();
    SetFocusedControl(nullptr);

    OnFocusLost(WindowFocusLostInfo{ shared_from_this(), message.Inner() });
}


void Window::OnFocusLost(const WindowFocusLostInfo& event_info) {

    auto observer = GetEventObserver<WindowFocusLostInfo>(
        GetPropertyMap(),
        FocusLostEventPropertyName);

    if (observer) {
        observer->OnNext(event_info);
    }
}


Observable<WindowFocusLostInfo> Window::FocusLostEvent() {
    return GetEventObservable<WindowFocusLostInfo>(
        GetPropertyMap(),
        FocusLostEventPropertyName);
}


void Window::HandleSizeMessage(const Message& message) {

    //Don't handle size message during window creation procedure,
    //because the object state is inconsistent until creation completed.
    if (!Handle()) {
        return;
    }

    zaf::Size size{
        static_cast<float>(LOWORD(message.LParam())),
        static_cast<float>(HIWORD(message.LParam()))
    };

    if (renderer_ != nullptr) {
        renderer_.Resize(size);
    }

    zaf::Rect root_control_rect{ Point(), ToDIPs(size, GetDPI()) };
    root_control_->SetRect(root_control_rect);

    UpdateWindowRect();
}


void Window::HandleMoveMessage() {

    UpdateWindowRect();
}


void Window::UpdateWindowRect() {

    RECT rect{};
    GetWindowRect(handle_, &rect);
    rect_ = ToDIPs(Rect::FromRECT(rect), GetDPI());
}


std::optional<LRESULT> Window::HandleWMNCCALCSIZE(const Message& message) {

    //WM_NCCALCSIZE must be passed to default window procedure if wparam is FALSE, no matter if
    //the window has customized style, otherwise the window could have some odd behaviors.
    if (message.WParam() == FALSE) {
        return std::nullopt;
    }

    //We need to remove the default window frame in WM_NCCALCSIZE for overlapped window without 
    //boder. It is no need to do that for popup window without boder.
    bool has_customized_style = !IsPopup() && !HasBorder();
    if (!has_customized_style) {
        return std::nullopt;
    }

    //Return TRUE to remove the default window frame.
    return TRUE;
}


std::optional<HitTestResult> Window::HitTest(const HitTestMessage& message) {

    auto mouse_over_control = [&]() {
        
        Point mouse_position = message.MousePosition();
        std::shared_ptr<Control> current_control = root_control_;
        while (true) {

            auto child = current_control->FindChildAtPosition(mouse_position);
            if (child == nullptr) {
                break;
            }

            auto child_position = child->Position();
            mouse_position.x += child_position.x;
            mouse_position.y += child_position.y;

            current_control = child;
        }
        return current_control;
    }();

    return mouse_over_control->HitTest(message);
}


bool Window::RedirectMouseWheelMessage(const Message& message) {

    HWND handle = GetCapture();
    if ((handle == nullptr) || (handle == Handle())) {
        return false;
    }

    auto capturing_mouse_window = GetWindowFromHandle(GetCapture());
    if (capturing_mouse_window == nullptr) {
        return false;
    }

    PostMessage(
        capturing_mouse_window->Handle(), 
        message.ID(),
        message.WParam(),
        message.LParam());

    return true;
}


bool Window::HandleMouseMessage(const MouseMessage& message) {

    Point position_at_begin_routing_control;
    auto begin_routing_control = GetBeginRoutingControlForMouseMessage(
        message, 
        position_at_begin_routing_control);

    if (message.ID() == WM_MOUSEMOVE || message.ID() == WM_NCMOUSEMOVE) {

        if (is_selecting_inspector_control_) {
            HighlightControlAtPosition(message.MousePosition());
            return true;
        }

        TrackMouseByMouseMove(message);

        begin_routing_control->FindMouseOverControl(position_at_begin_routing_control, message);
    }
    else {

        HideTooltipWindow();

        if (message.ID() == WM_LBUTTONDOWN || message.ID() == WM_RBUTTONDOWN) {

            if (is_selecting_inspector_control_) {
                SelectInspectedControl();
                return true;
            }
        }
    }

    return RouteMouseEvent(begin_routing_control, position_at_begin_routing_control, message);
}


std::shared_ptr<Control> Window::GetBeginRoutingControlForMouseMessage(
    const MouseMessage& message,
    Point& position_at_control) const {

    std::shared_ptr<Control> result;

    if (mouse_capture_control_) {

        result = mouse_capture_control_;

        position_at_control = TranslateAbsolutePositionToControlPosition(
            message.MousePosition(),
            *result);
    }
    else {

        result = root_control_;
        position_at_control = message.MousePosition();
    }

    return result;
}


bool Window::RouteMouseEvent(
    const std::shared_ptr<Control>& begin_routing_control,
    const Point& position_at_begin_routing_control, 
    const MouseMessage& message) {

    auto event_source_control = begin_routing_control->FindEnabledControlAtPosition(
        position_at_begin_routing_control);

    if (!event_source_control) {
        return false;
    }

    switch (message.ID()) {
    case WM_MOUSEMOVE:
        return RouteMouseEventGeneric<MouseMoveInfo>(
            event_source_control,
            message,
            &Control::OnMouseMove);

    case WM_LBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
        return RouteMouseEventGeneric<MouseDownInfo>(
            event_source_control,
            message,
            &Control::OnMouseDown);

    case WM_LBUTTONUP:
    case WM_NCLBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        return RouteMouseEventGeneric<MouseUpInfo>(
            event_source_control,
            message,
            &Control::OnMouseUp);

    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
        return RouteMouseEventGeneric<MouseWheelInfo>(
            event_source_control,
            message,
            &Control::OnMouseWheel);
        
    default:
        return false;
    }
}


void Window::HighlightControlAtPosition(const Point& position) {

    auto highlight_control = root_control_->FindChildAtPositionRecursively(position);
    if (!highlight_control) {
        highlight_control = root_control_;
    }

    SetHighlightControl(highlight_control);

    auto inspector_port = GetInspectorPort();
    if (inspector_port) {
        inspector_port->HighlightControl(highlight_control);
    }
}


void Window::SelectInspectedControl() {

    is_selecting_inspector_control_ = false;

    if (!highlight_control_) {
        return;
    }

    auto inspector_port = GetInspectorPort();
    if (inspector_port) {
        inspector_port->SelectControl(highlight_control_); 
    }
    
    SetHighlightControl(nullptr);
}


void Window::TrackMouseByMouseMove(const MouseMessage& message) {

    bool is_non_client = message.ID() == WM_NCMOUSEMOVE;

    auto is_tracking_mouse = [this, is_non_client]() {

        if (track_mouse_mode_ == TrackMouseMode::ClientArea &&
            !is_non_client) {
            return true;
        }

        if (track_mouse_mode_ == TrackMouseMode::NonClientArea &&
            is_non_client) {
            return true;
        }

        return false;
    }();

    if (is_tracking_mouse) {
        return;
    }

    TrackMouse(is_non_client);
}


void Window::TrackMouse(bool is_non_client) {

    //Don't track mouse if the mouse is being captured and the caputring window is not current 
    //window. This would happend if WM_MOUSEMOVE message is redirected from a capturing window to 
    //another window (this is the way how PopupMenu works).
    auto capturing_window = GetCapture();
    if (capturing_window && capturing_window != Handle()) {
        return;
    }

    TRACKMOUSEEVENT track_mouse_event_param {};
    track_mouse_event_param.cbSize = sizeof(track_mouse_event_param);
    track_mouse_event_param.dwFlags = TME_LEAVE | TME_HOVER;
    if (is_non_client) {
        track_mouse_event_param.dwFlags |= TME_NONCLIENT;
    }
    track_mouse_event_param.hwndTrack = handle_;

    if (TrackMouseEvent(&track_mouse_event_param)) {

        track_mouse_mode_ =
            is_non_client ?
            TrackMouseMode::NonClientArea :
            TrackMouseMode::ClientArea;
    }
}


void Window::OnMouseHover(const Message& message) {

    if (mouse_over_control_) {

        //Raise and route mouse hover event.
        auto event_info_state = std::make_shared<RoutedEventSharedState>(mouse_over_control_);
        for (auto control = mouse_over_control_; control; control = control->Parent()) {
            control->OnMouseHover(MouseHoverInfo{ event_info_state, control });
        }
    }

    TryToShowTooltipWindow();
}


void Window::OnMouseLeave(const Message& message) {

    bool is_tracking_mouse = [&]() {
    
        if (track_mouse_mode_ == TrackMouseMode::ClientArea &&
            message.ID() == WM_MOUSELEAVE) {
            return true;
        }

        if (track_mouse_mode_ == TrackMouseMode::NonClientArea &&
            message.ID() == WM_NCMOUSELEAVE) {
            return true;
        }

        return false;
    }();

    if (is_tracking_mouse) {
        track_mouse_mode_ = TrackMouseMode::None;
        SetMouseOverControl(nullptr, MouseMessage{ Message{} });
    }
}


bool Window::ChangeMouseCursor(const Message& message) {

    bool is_changed = false;

    if (mouse_over_control_ != nullptr) {
        mouse_over_control_->ChangeMouseCursor(message, is_changed);
    }

    return is_changed;
}


bool Window::OnKeyDown(const Message& message) {
    return RouteKeyboardEventGeneric<KeyDownInfo>(focused_control_, message, &Control::OnKeyDown);
}


bool Window::OnKeyUp(const Message& message) {
    return RouteKeyboardEventGeneric<KeyUpInfo>(focused_control_, message, &Control::OnKeyUp);
}


bool Window::OnCharInput(const Message& message) {
    return RouteKeyboardEventGeneric<CharInputInfo>(
        focused_control_, 
        message,
        &Control::OnCharInput);
}


void Window::TryToShowTooltipWindow() {

    if (!mouse_over_control_) {
        return;
    }

    auto tooltip = mouse_over_control_->Tooltip();
    if (tooltip.empty()) {
        return;
    }

    if (!tooltip_window_) {
        tooltip_window_ = zaf::Create<zaf::TooltipWindow>();
        tooltip_window_->SetOwner(shared_from_this());
    }

    tooltip_window_->SetText(tooltip);
    tooltip_window_->ShowBelowMouseCursor();
}


void Window::HideTooltipWindow() {
    if (tooltip_window_) {
        tooltip_window_->Hide();
    }
}


bool Window::HandleWMCLOSE() {

    ClosingInfo event_info{ shared_from_this() };
    OnClosing(event_info);
    return event_info.CanClose();
}


void Window::OnClosing(const ClosingInfo& event_info) {

    auto observer = GetEventObserver<ClosingInfo>(GetPropertyMap(), ClosingEventPropertyName);
    if (observer) {
        observer->OnNext(event_info);
    }
}


void Window::HandleWMDESTROY() {

    //Avoid reentering.
    if (is_being_destroyed_.Value()) {
        return;
    }

    auto guard = is_being_destroyed_.BeginSet(true);

    CancelMouseCapture();

    if (focused_control_ != nullptr) {
        focused_control_->SetIsFocusedByWindow(false);
        focused_control_ = nullptr;
    }

    root_control_->ReleaseRendererResources();

    HWND old_handle = handle_;

    handle_ = nullptr;
    renderer_.Reset();
    tooltip_window_.reset();

    OnDestroyed(DestroyedInfo{ shared_from_this(), old_handle });
}


void Window::OnDestroyed(const DestroyedInfo& event_info) {

    auto event_observer = GetEventObserver<DestroyedInfo>(
        GetPropertyMap(),
        DestroyedEventPropertyName);

    if (event_observer) {
        event_observer->OnNext(event_info);
    }
}


void Window::HandleWMNCDESTROY() {

    auto holder = holder_.lock();
    if (holder) {

        Application::Instance().UnregisterShownWindow(holder);

        holder->Detach();
        holder_.reset();
    }
}


void Window::SetMouseOverControl(
    const std::shared_ptr<Control>& mouse_over_control, 
    const MouseMessage& message) {

    if (mouse_over_control_ == mouse_over_control) {
        return;
    }

    if (mouse_over_control != nullptr) {

        //The mouse over control must be contained in this window
        if (mouse_over_control->Window().get() != this) {
            return;
        }
    }

    auto old_control = mouse_over_control_;
    mouse_over_control_ = mouse_over_control;

    if (old_control ) {
        ChangeControlMouseOverState(old_control, false, mouse_over_control_);
    }

    if (mouse_over_control_ != nullptr) {

        //Window finds the mouse over control when received WM_MOUSEMOVE,
        //but WM_SETCURSOR is always received before WM_MOUSEMOVE, in such
        //case the mouse over control cannot change the cursor promptly. So 
        //here, a simulated WM_SETCURSOR is posted to give a change to 
        //change the cursor immediately.
        PostMessage(
            Handle(),
            WM_SETCURSOR,
            reinterpret_cast<WPARAM>(Handle()),
            MAKELPARAM(message.HitTestResult(), message.ID()));

        ChangeControlMouseOverState(mouse_over_control_, true, old_control);

        //Track mouse again to generate next mouse hover message.
        //TODO: Find out whether the new control is in non-client area.
        TrackMouse(false);
    }

    HideTooltipWindow();
}


void Window::ChangeControlMouseOverState(
    const std::shared_ptr<Control>& target_control,
    bool is_mouse_over,
    const std::shared_ptr<Control>& changed_control) {

    target_control->SetIsMouseOverByWindow(is_mouse_over);

    //Raise and route event.
    auto event_info_state = std::make_shared<internal::MouseOverEventSharedState>(
        target_control, 
        changed_control);

    for (auto control = target_control; control; control = control->Parent()) {

        if (is_mouse_over) {
            control->OnMouseEnter(MouseEnterInfo{ event_info_state, control });
        }
        else {
            control->OnMouseLeave(MouseLeaveInfo{ event_info_state, control });
        }
    }
}


void Window::SetMouseCaptureControl(
    const std::shared_ptr<Control>& capture_control,
    bool is_releasing) {

    if (capture_control == nullptr) {
        return;
    }

    if (!is_releasing) {
        CaptureMouseWithControl(capture_control);
    }
    else {
        ReleaseMouseWithControl(capture_control);
    }
}


void Window::CaptureMouseWithControl(const std::shared_ptr<Control>& control) {
    
    auto previous_control = mouse_capture_control_;
    if (previous_control == control) {
        return;
    }

    //The control must be contained in this window.
    if (control->Window().get() != this) {
        return;
    }

    if (previous_control != nullptr) {
        previous_control->IsCapturingMouseChanged(false);
    }
    else {
        SetCapture(Handle());
    }

    mouse_capture_control_ = control;
    mouse_capture_control_->IsCapturingMouseChanged(true);

    OnMouseCaptureControlChanged(MouseCaptureControlChangedInfo{ 
        shared_from_this(),
        previous_control 
    });
}


void Window::ReleaseMouseWithControl(const std::shared_ptr<Control>& control) {

    if (mouse_capture_control_ != control) {
        return;
    }

    //ReleaseCapture sends a WM_CAPTURECAHNGED message to the window, 
    //in which message handler will set mouse_capture_control_ to nullptr.
    ReleaseCapture();
}


void Window::CancelMouseCapture() {

    if (!mouse_capture_control_) {
        return;
    }

    //Set mouse_capture_control_ to nullptr before raising events to avoid reentering issues.
    auto previous_control = mouse_capture_control_;
    mouse_capture_control_ = nullptr;
    previous_control->IsCapturingMouseChanged(false);

    OnMouseCaptureControlChanged(MouseCaptureControlChangedInfo{
        shared_from_this(),
        previous_control
    });
}


void Window::OnMouseCaptureControlChanged(const MouseCaptureControlChangedInfo& event_info) {

    auto observer = GetEventObserver<MouseCaptureControlChangedInfo>(
        GetPropertyMap(), 
        MouseCaptureControlChangedEventPropertyName);

    if (observer) {
        observer->OnNext(event_info);
    }
}


Observable<MouseCaptureControlChangedInfo> Window::MouseCaptureControlChangedEvent() {
    return GetEventObservable<MouseCaptureControlChangedInfo>(
        GetPropertyMap(), 
        MouseCaptureControlChangedEventPropertyName);
}


void Window::SetFocusedControl(const std::shared_ptr<Control>& new_focused_control) {

    auto previous_focused_control = focused_control_;
    if (previous_focused_control == new_focused_control) {
        return;
    }

    if (new_focused_control) {

        //Not allow to set focused control if the window has no focus.
        //(But allow to clear focused control)
        if (!IsFocused()) {
            return;
        }

        if (!new_focused_control->IsEnabled()) {
            return;
        }

        //The focused control must be contained in this window
        if (new_focused_control->Window().get() != this) {
            return;
        }
    }

    focused_control_ = new_focused_control;

    if (previous_focused_control) {
        ChangeControlFocusState(previous_focused_control, new_focused_control, false);
    }

    if (new_focused_control) {
        ChangeControlFocusState(new_focused_control, previous_focused_control, true);
    }

    OnFocusedControlChanged(previous_focused_control);
}


void Window::ChangeControlFocusState(
    const std::shared_ptr<Control>& target_control, 
    const std::shared_ptr<Control>& changing_control,
    bool is_focused) {

    target_control->SetIsFocusedByWindow(is_focused);

    if (is_focused) {

        RouteFocusEventGeneric<FocusGainedInfo>(
            target_control,
            changing_control,
            &Control::OnFocusGained);
    }
    else {

        RouteFocusEventGeneric<FocusLostInfo>(
            target_control, 
            changing_control,
            &Control::OnFocusLost);
    }
}


std::shared_ptr<Window> Window::Owner() const {

    auto owner = GetPropertyMap().TryGetProperty<std::weak_ptr<Window>>(kOwnerPropertyName);
    if (owner) {
        return owner->lock();
    }
    else {
        return nullptr;
    }
}


void Window::SetOwner(const std::shared_ptr<Window>& owner) {

    if (!Handle()) {

        std::weak_ptr<Window> weak_owner = owner;
        GetPropertyMap().SetProperty(kOwnerPropertyName, weak_owner);
    }
}


InitialRectStyle Window::InitialRectStyle() const {

    auto style = 
        GetPropertyMap().TryGetProperty<zaf::InitialRectStyle>(kInitialRectStylePropertyName);

    if (style != nullptr) {
        return *style;
    }
    else {
        return InitialRectStyle::CenterInScreen;
    }
}


void Window::SetInitialRectStyle(zaf::InitialRectStyle initial_rect_style) {

    GetPropertyMap().SetProperty(kInitialRectStylePropertyName, initial_rect_style);
}


Rect Window::Rect() const {

    if (!Handle()) {
        return rect_;
    }
    else {

        RECT rect{};
        GetWindowRect(handle_, &rect);
        return ToDIPs(Rect::FromRECT(rect), GetDPI());
    }
}


void Window::SetRect(const zaf::Rect& rect) {

    if (!Handle()) {
        rect_ = rect;
    }
    else {

        auto new_rect = ToAlignedPixelsRect(rect, GetDPI());

        SetWindowPos(
            handle_,
            nullptr,
            static_cast<int>(new_rect.position.x),
            static_cast<int>(new_rect.position.y),
            static_cast<int>(new_rect.size.width),
            static_cast<int>(new_rect.size.height),
            SWP_NOZORDER | SWP_NOACTIVATE);
    }
}


zaf::Rect Window::ContentRect() const {
    return zaf::Rect{ Point{}, ContentSize() };
}


zaf::Size Window::ContentSize() const {

    if (Handle()) {

        RECT client_rect{};
        ::GetClientRect(handle_, &client_rect);
        return ToDIPs(Rect::FromRECT(client_rect).size, GetDPI());
    }
    else {

        auto adjusted_size = AdjustContentSizeToWindowSize(zaf::Size{});

        auto result = Size();
        result.width = std::max(result.width - adjusted_size.width, 0.f);
        result.height = std::max(result.height - adjusted_size.height, 0.f);
        return result;
    }
}


void Window::SetContentSize(const zaf::Size& size) {

    zaf::Rect new_rect;
    new_rect.position = Position();
    new_rect.size = AdjustContentSizeToWindowSize(size);

    SetRect(new_rect);
}


zaf::Size Window::AdjustContentSizeToWindowSize(const zaf::Size& content_size) const {

    auto dpi = GetDPI();

    zaf::Rect rect{ zaf::Point{}, content_size };
    auto rounded_rect = ToAlignedPixelsRect(rect, dpi);
    auto adjusted_rect = rounded_rect.ToRECT();

    DWORD style{};
    DWORD extra_style{};
    GetHandleStyles(style, extra_style);

    BOOL is_succeeded = AdjustWindowRectExForDpi(
        &adjusted_rect, 
        style, 
        FALSE, 
        extra_style,
        static_cast<UINT>(dpi));

    if (!is_succeeded) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    auto result = zaf::Size{
        static_cast<float>(adjusted_rect.right - adjusted_rect.left),
        static_cast<float>(adjusted_rect.bottom - adjusted_rect.top)
    };

    result = ToDIPs(result, dpi);
    return result;
}


float Window::MinWidth() const {

    auto width = GetPropertyMap().TryGetProperty<float>(property::MinWidth);
    if (width != nullptr) {
        return *width;
    }
    return static_cast<float>(GetSystemMetrics(SM_CXMINTRACK));
}


void Window::SetMinWidth(float min_width) {

    GetPropertyMap().SetProperty(property::MinWidth, min_width);

    if (MaxWidth() < min_width) {
        SetMaxWidth(min_width);
    }

    if (Width() < min_width) {
        SetWidth(min_width);
    }
}


float Window::MaxWidth() const {

    auto width = GetPropertyMap().TryGetProperty<float>(property::MaxWidth);
    if (width != nullptr) {
        return *width;
    }
    return static_cast<float>(GetSystemMetrics(SM_CXMAXTRACK));
}


void Window::SetMaxWidth(float max_width) {

    GetPropertyMap().SetProperty(property::MaxWidth, max_width);

    if (MinWidth() > max_width) {
        SetMinWidth(max_width);
    }

    if (Width() > max_width) {
        SetWidth(max_width);
    }
}


float Window::MinHeight() const {

    auto height = GetPropertyMap().TryGetProperty<float>(property::MinHeight);
    if (height != nullptr) {
        return *height;
    }
    return static_cast<float>(GetSystemMetrics(SM_CYMINTRACK));
}


void Window::SetMinHeight(float min_height) {

    GetPropertyMap().SetProperty(property::MinHeight, min_height);

    if (MaxHeight() < min_height) {
        SetMaxHeight(min_height);
    }

    if (Height() < min_height) {
        SetHeight(min_height);
    }
}


float Window::MaxHeight() const {

    auto height = GetPropertyMap().TryGetProperty<float>(property::MaxHeight);
    if (height != nullptr) {
        return *height;
    }
    return static_cast<float>(GetSystemMetrics(SM_CYMAXTRACK));
}


void Window::SetMaxHeight(float max_height) {

    GetPropertyMap().SetProperty(property::MaxHeight, max_height);

    if (MinHeight() > max_height) {
        SetMinHeight(max_height);
    }

    if (Height() > max_height) {
        SetHeight(max_height);
    }
}


ActivateOption Window::ActivateOption() const {

    auto option = GetPropertyMap().TryGetProperty<zaf::ActivateOption>(kActivateOptionPropertyName);
    if (option != nullptr) {
        return *option;
    }
    else {
        return ActivateOption::Normal;
    }
}

void Window::SetActivateOption(zaf::ActivateOption option) {

    if (!Handle()) {
        GetPropertyMap().SetProperty(kActivateOptionPropertyName, option);
    }
}


bool Window::IsPopup() const {
    auto value = GetPropertyMap().TryGetProperty<bool>(kIsPopupPropertyName);
    return value ? *value : false;
}

void Window::SetIsPopup(bool is_popup) {

    if (!Handle()) {
        GetPropertyMap().SetProperty(kIsPopupPropertyName, is_popup);
        ReviseHasTitleBar();
    }
}


bool Window::HasBorder() const {
    auto value = GetPropertyMap().TryGetProperty<bool>(kHasBorderPropertyName);
    return value ? *value : true;
}

void Window::SetHasBorder(bool has_border) {

    if (!Handle()) {
        GetPropertyMap().SetProperty(kHasBorderPropertyName, has_border);
        ReviseHasTitleBar();
    }
}


bool Window::HasTitleBar() const {
    auto value = GetPropertyMap().TryGetProperty<bool>(kHasTitleBarPropertyName);
    return value ? *value : true;
}


void Window::SetHasTitleBar(bool has_title_bar) {

    //If the handle has been created, and it is an overlapped window and has border,
    //not allow to change this property.
    //If the handle is not created yet, allow to change this property, but it will be
    //revised when creating handle.
    if (!!Handle()) {
        if (!IsPopup() && HasBorder()) {
            return;
        }
    }

    GetPropertyMap().SetProperty(kHasTitleBarPropertyName, has_title_bar);

    if (HasBorder()) {
        SetStyleToHandle(WS_CAPTION, has_title_bar, false);
    }
}


void Window::ReviseHasTitleBar() {
    if (!IsPopup() && HasBorder()) {
        SetHasTitleBar(true);
    }
}


bool Window::IsSizable() const {
    auto value = GetPropertyMap().TryGetProperty<bool>(kIsSizablePropertyName);
    return value ? *value : true;
}

void Window::SetIsSizable(bool is_sizable) {

    GetPropertyMap().SetProperty(kIsSizablePropertyName, is_sizable);

    if (HasBorder()) {
        SetStyleToHandle(WS_SIZEBOX, is_sizable, false);
    }
}


bool Window::HasSystemMenu() const {
    auto value = GetPropertyMap().TryGetProperty<bool>(kHasSystemMenuPropertyName);
    return value ? *value : true;
}

void Window::SetHasSystemMenu(bool has_system_menu) {
    SetStyleProperty(kHasSystemMenuPropertyName, WS_SYSMENU, has_system_menu, false);
}


bool Window::CanMinimize() const {
    auto value = GetPropertyMap().TryGetProperty<bool>(kCanMinimizePropertyName);
    return value ? *value : true;
}

void Window::SetCanMinimize(bool can_minimize) {
    SetStyleProperty(kCanMinimizePropertyName, WS_MINIMIZEBOX, can_minimize, false);
}


bool Window::CanMaximize() const {
    auto value = GetPropertyMap().TryGetProperty<bool>(kCanMaximizePropertyName);
    return value ? *value : true;
}

void Window::SetCanMaximize(bool has_maximize_button) {
    SetStyleProperty(kCanMaximizePropertyName, WS_MAXIMIZEBOX, has_maximize_button, false);
}


bool Window::IsToolWindow() const {

    auto is_set = GetPropertyMap().TryGetProperty<bool>(kIsToolWindowPropertyName);
    if (is_set != nullptr) {
        return *is_set;
    }
    return false;
}

void Window::SetIsToolWindow(bool is_tool_window) {
    SetStyleProperty(kIsToolWindowPropertyName, WS_EX_TOOLWINDOW, is_tool_window, true);
}


bool Window::IsTopmost() const {

    auto is_set = GetPropertyMap().TryGetProperty<bool>(kIsTopmostPropertyName);
    if (is_set) {
        return *is_set;
    }
    return false;
}


void Window::SetIsTopmost(bool is_topmost) {
    SetStyleProperty(kIsTopmostPropertyName, WS_EX_TOPMOST, is_topmost, true);
}


void Window::SetStyleProperty(
    const std::wstring& property_name,
    DWORD style_value,
    bool is_set,
    bool is_extra_style) {

    GetPropertyMap().SetProperty(property_name, is_set);

    if (! !Handle()) {
        SetStyleToHandle(style_value, is_set, is_extra_style);
    }
}


void Window::SetStyleToHandle(DWORD style_value, bool is_set, bool is_extra_style) {

    DWORD category = is_extra_style ? GWL_EXSTYLE : GWL_STYLE;

    DWORD style = GetWindowLong(Handle(), category);
    if (is_set) {
        style |= style_value;
    }
    else {
        style &= ~style_value;
    }
    SetWindowLong(Handle(), category, style);
}


std::wstring Window::Title() const {

    if (!Handle()) {

        auto title = GetPropertyMap().TryGetProperty<std::wstring>(kTitlePropertyName);
        if (title != nullptr) {
            return *title;
        }
        else {
            return std::wstring();
        }
    }
    else {

        int title_length = GetWindowTextLength(handle_);
        std::vector<wchar_t> buffer(title_length + 1);
        GetWindowText(handle_, buffer.data(), static_cast<int>(buffer.size()));
        return buffer.data();
    }
}


void Window::SetTitle(const std::wstring& title) {

    GetPropertyMap().SetProperty(kTitlePropertyName, title);

    if (! !Handle()) {
        SetWindowText(handle_, title.c_str());
    }
}


void Window::SetRootControl(const std::shared_ptr<Control>& control) {
    InitializeRootControl(control != nullptr ? control : Create<Control>());
}


void Window::InitializeRootControl(const std::shared_ptr<Control>& control) {

    if (root_control_ == control) {
        return;
    }

    auto previous_root_control = root_control_;
    if (previous_root_control != nullptr) {
        previous_root_control->ReleaseRendererResources();
        previous_root_control->SetWindow(nullptr);
    }

    root_control_ = control;
    root_control_->SetWindow(shared_from_this());

    if (!!Handle()) {
        RECT client_rect{};
        ::GetClientRect(handle_, &client_rect);
        root_control_->SetRect(Rect::FromRECT(client_rect));
    }

    OnRootControlChanged(RootControlChangedInfo{ shared_from_this(), previous_root_control });
}


void Window::OnRootControlChanged(const RootControlChangedInfo& event_info) {
    
    auto observer = GetEventObserver<RootControlChangedInfo>(
        GetPropertyMap(),
        RootControlChangedEventPropertyName);

    if (observer) {
        observer->OnNext(event_info);
    }
}


Observable<RootControlChangedInfo> Window::RootControlChangedEvent() {
    return GetEventObservable<RootControlChangedInfo>(
        GetPropertyMap(), 
        RootControlChangedEventPropertyName);
}


Observable<ClosingInfo> Window::ClosingEvent() {
    return GetEventObservable<ClosingInfo>(GetPropertyMap(), ClosingEventPropertyName);
}


Observable<DestroyedInfo> Window::DestroyedEvent() {
    return GetEventObservable<DestroyedInfo>(
        GetPropertyMap(), 
        DestroyedEventPropertyName);
}


Observable<MessageReceivedInfo> Window::MessageReceivedEvent() {
    return GetEventObservable<MessageReceivedInfo>(
        GetPropertyMap(), 
        MessageReceivedEventPropertyName);
}


Observable<MessageHandledInfo> Window::MessageHandledEvent() {
    return GetEventObservable<MessageHandledInfo>(
        GetPropertyMap(), 
        MessageHandledEventPropertyName);
}


Point Window::GetMousePosition() const {

    POINT cursor_point = { 0 };
    GetCursorPos(&cursor_point);
    ScreenToClient(handle_, &cursor_point);

    Point point_in_pixels{ 
        static_cast<float>(cursor_point.x), 
        static_cast<float>(cursor_point.y) 
    };

    return ToDIPs(point_in_pixels, GetDPI());
}


Point Window::ToScreenPosition(const Point& position_in_window) const {

    auto dpi = GetDPI();
    auto position_in_pixel = FromDIPs(position_in_window, dpi);

    auto point = position_in_pixel.ToPOINT();
    ClientToScreen(Handle(), &point);

    auto result = Point::FromPOINT(point);
    return ToDIPs(result, dpi);
}


Point Window::FromScreenPosition(const Point& position_in_screen) const {

    auto dpi = GetDPI();
    auto position_in_pixel = FromDIPs(position_in_screen, dpi);

    auto point = position_in_pixel.ToPOINT();
    ScreenToClient(Handle(), &point);

    auto result = Point::FromPOINT(point);
    return ToDIPs(result, dpi);
}


float Window::GetDPI() const {

    if (Handle()) {
        return static_cast<float>(GetDpiForWindow(Handle()));
    }
    else {
        return Application::Instance().GetSystemDPI();
    }
}


void Window::Show() {

    auto holder = CreateWindowHandleIfNeeded();
    Application::Instance().RegisterShownWindow(holder);

    auto activate_option = ActivateOption();
    bool no_activate = (activate_option & ActivateOption::NoActivate) == ActivateOption::NoActivate;
    ShowWindow(handle_, no_activate ? SW_SHOWNA : SW_SHOW);
}


void Window::Hide() {
    ShowWindow(handle_, SW_HIDE);
}


void Window::Maximize() {
    ShowWindow(handle_, SW_MAXIMIZE);
}


void Window::Minimize() {
    ShowWindow(handle_, SW_MINIMIZE);
}


void Window::Restore() {
    ShowWindow(handle_, SW_RESTORE);
}


bool Window::IsVisible() const {
    return !!IsWindowVisible(handle_);
}


bool Window::IsFocused() const {
    return GetFocus() == Handle();
}


void Window::Close() {

    //Do nothing if the window is being destroyed.
    if (is_being_destroyed_.Value()) {
        return;
    }

    SendMessage(Handle(), WM_CLOSE, 0, 0);
}


void Window::Destroy() {
    if (handle_) {
        DestroyWindow(handle_);
    }
}


void Window::ShowInspectorWindow() {

    if (inspector_window_.lock()) {
        return;
    }

    auto inspector_window = Create<InspectorWindow>(shared_from_this());
    inspector_window->Show();

    inspector_window_ = inspector_window;
}


void Window::SetHighlightControl(const std::shared_ptr<Control>& highlight_control) {

    if (highlight_control_ == highlight_control) {
        return;
    }

    if (!highlight_control) {
        highlight_control_ = nullptr;
        NeedRepaintRect(root_control_->Rect());
        return;
    }

    if (highlight_control->Window().get() != this) {
        return;
    }

    //Repaint the rect of previous highlight control.
    if (highlight_control_) {
        NeedRepaintRect(highlight_control_->AbsoluteRect());
    }

    highlight_control_ = highlight_control;

    //Repaint the rect of new highlight control.
    NeedRepaintRect(highlight_control_->AbsoluteRect());
}


std::shared_ptr<internal::InspectorPort> Window::GetInspectorPort() const {

    auto inspector_window = inspector_window_.lock();
    if (!inspector_window) {
        return nullptr;
    }

    return inspector_window->GetPort();
}


void Window::BeginSelectInspectedControl() {

    is_selecting_inspector_control_ = true;
}


std::shared_ptr<Window> GetWindowFromHandle(HWND handle) {

    if (handle == nullptr) {
        return nullptr;
    }

    LONG_PTR user_data = GetWindowLongPtr(handle, GWLP_USERDATA);
    Window* window = reinterpret_cast<Window*>(user_data);
    if (window == nullptr) {
        return nullptr;
    }

    return window->shared_from_this();
}

}