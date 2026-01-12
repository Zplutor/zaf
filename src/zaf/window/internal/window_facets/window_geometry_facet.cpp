#include <zaf/window/internal/window_facets/window_geometry_facet.h>
#include <zaf/base/auto_reset.h>
#include <zaf/base/error/win32_error.h>
#include <zaf/graphic/dpi.h>
#include <zaf/graphic/pixel_snapping.h>
#include <zaf/window/event/dpi_changed_info.h>
#include <zaf/window/internal/window_facets/window_lifecycle_facet.h>
#include <zaf/window/internal/window_not_created_state_data.h>
#include <zaf/window/invalid_handle_state_error.h>
#include <zaf/window/screen_manager.h>
#include <zaf/window/window.h>

namespace zaf::internal {

std::shared_ptr<zaf::Screen> WindowGeometryFacet::ResolveInitialScreen(
    const internal::WindowNotCreatedStateData& state_data, 
    const std::shared_ptr<Window>& owner) noexcept {

    auto screen = state_data.screen.lock();
    if (screen) {
        return screen;
    }

    if (owner) {
        return owner->Screen();
    }

    return ScreenManager::Instance().PrimaryScreen();
}


zaf::Rect WindowGeometryFacet::ResolveInitialRect(
    const zaf::Screen& screen, 
    const std::shared_ptr<Window>& owner, 
    const internal::WindowNotCreatedStateData& state_data) noexcept {

    // The window has a specified initial position, use it.
    if (state_data.initial_position) {
        return zaf::Rect{
            *state_data.initial_position,
            state_data.size
        };
    }

    // The window doesn't have a specified initial position, use the center position of its owner 
    // if there is one.
    if (owner) {
        auto owner_rect = owner->Rect();
        Point position{
            owner_rect.position.x + (owner_rect.size.width - state_data.size.width) / 2,
            owner_rect.position.y + (owner_rect.size.height - state_data.size.height) / 2
        };
        auto result = zaf::Rect{ position, state_data.size };
        return SnapToPixels(result, screen.DPI());
    }

    // Use the center position of the screen.
    auto screen_rect = screen.RectInSelf();
    Point position{
        (screen_rect.size.width - state_data.size.width) / 2,
        (screen_rect.size.height - state_data.size.height) / 2
    };
    auto result = zaf::Rect{ position, state_data.size };
    return SnapToPixels(result, screen.DPI());
}


WindowGeometryFacet::WindowGeometryFacet(Window& window) noexcept : window_(window) {

}


std::shared_ptr<zaf::Screen> WindowGeometryFacet::Screen() const noexcept {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::NotCreated) {
        return ResolveInitialScreen(
            window_.lifecycle_facet_->NotCreatedStateData(), 
            window_.Owner());
    }
    else if (handle_state == WindowHandleState::Creating ||
             handle_state == WindowHandleState::Created ||
             handle_state == WindowHandleState::Destroying) {

        HMONITOR screen_handle = MonitorFromWindow(window_.Handle(), MONITOR_DEFAULTTOPRIMARY);
        return ScreenManager::Instance().ScreenFromHandle(screen_handle);
    }
    else {
        return ScreenManager::Instance().PrimaryScreen();
    }
}


void WindowGeometryFacet::SetScreen(std::shared_ptr<zaf::Screen> screen) {

    ZAF_EXPECT(screen);

    if (window_.HandleState() != WindowHandleState::NotCreated) {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    window_.lifecycle_facet_->NotCreatedStateData().screen = std::move(screen);
}


float WindowGeometryFacet::DPI() const noexcept {
    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::Creating ||
        handle_state == WindowHandleState::Created ||
        handle_state == WindowHandleState::Destroying) {
        return static_cast<float>(GetDpiForWindow(window_.Handle()));
    }
    return this->Screen()->DPI();
}


zaf::Rect WindowGeometryFacet::Rect() const noexcept {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::NotCreated) {

        return ResolveInitialRect(
            *this->Screen(),
            window_.Owner(), 
            window_.lifecycle_facet_->NotCreatedStateData());
    }

    if (handle_state == WindowHandleState::Creating ||
        handle_state == WindowHandleState::Created ||
        handle_state == WindowHandleState::Destroying) {

        RECT rect{};
        GetWindowRect(window_.Handle(), &rect);
        return this->Screen()->TransformFromGlobal(Rect::FromRECT(rect));
    }

    return {};
}


void WindowGeometryFacet::SetRect(const zaf::Rect& rect) {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::NotCreated) {

        zaf::Rect new_rect{ rect.position, ClampSize(rect.size) };
        new_rect = SnapToPixels(new_rect, DPI());

        auto& not_created_state_data = window_.lifecycle_facet_->NotCreatedStateData();
        not_created_state_data.initial_position = new_rect.position;

        if (not_created_state_data.size != new_rect.size) {
            not_created_state_data.size = new_rect.size;
            RaiseSizeChangedEvent();
        }
    }
    else if (handle_state == WindowHandleState::Creating ||
             handle_state == WindowHandleState::Created) {

        auto new_rect = rect;
        new_rect.size = ClampSize(new_rect.size);
        new_rect = SnapToPixels(new_rect, DPI());
        auto new_rect_in_global = this->Screen()->TransformToGlobal(new_rect);

        BOOL is_succeeded = SetWindowPos(
            window_.Handle(),
            nullptr,
            static_cast<int>(new_rect_in_global.position.x),
            static_cast<int>(new_rect_in_global.position.y),
            static_cast<int>(new_rect_in_global.size.width),
            static_cast<int>(new_rect_in_global.size.height),
            SWP_NOZORDER | SWP_NOACTIVATE);

        if (!is_succeeded) {
            ZAF_THROW_WIN32_ERROR(GetLastError());
        }
    }
    else {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }
}


zaf::Size WindowGeometryFacet::ClampSize(const zaf::Size& size) const noexcept {
    zaf::Size result;
    result.width = std::clamp(size.width, MinWidth(), MaxWidth());
    result.height = std::clamp(size.height, MinHeight(), MaxHeight());
    return result;
}


Point WindowGeometryFacet::Position() const noexcept {
    return Rect().position;
}


void WindowGeometryFacet::SetPosition(const Point& position) {
    zaf::Rect new_rect = Rect();
    new_rect.position = position;
    SetRect(new_rect);
}


zaf::Size WindowGeometryFacet::Size() const noexcept {
    return Rect().size;
}


void WindowGeometryFacet::SetSize(const zaf::Size& size) {

    if (window_.HandleState() == WindowHandleState::NotCreated) {

        auto new_size = ClampSize(size);

        auto& not_created_state_data = window_.lifecycle_facet_->NotCreatedStateData();
        if (not_created_state_data.size != new_size) {
            not_created_state_data.size = new_size;
            RaiseSizeChangedEvent();
        }
    }
    else {
        zaf::Rect new_rect = Rect();
        new_rect.size = size;
        SetRect(new_rect);
    }
}


float WindowGeometryFacet::Width() const noexcept {
    return Size().width;
}


void WindowGeometryFacet::SetWidth(float width) {
    zaf::Size new_size = Size();
    new_size.width = width;
    SetSize(new_size);
}


float WindowGeometryFacet::Height() const noexcept {
    return Size().height;
}


void WindowGeometryFacet::SetHeight(float height) {
    zaf::Size new_size = Size();
    new_size.height = height;
    SetSize(new_size);
}


float WindowGeometryFacet::MinWidth() const noexcept {
    if (min_width_) {
        return *min_width_;
    }
    return ToDIPs(static_cast<float>(GetSystemMetrics(SM_CXMINTRACK)), DPI());
}


void WindowGeometryFacet::SetMinWidth(float min_width) {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::Destroying ||
        handle_state == WindowHandleState::Destroyed) {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    auto backup_min_width = min_width_;
    auto backup_max_width = max_width_;

    min_width_ = min_width;

    if (MaxWidth() < min_width) {
        max_width_ = min_width;
    }

    try {
        if (Width() < min_width) {
            SetWidth(min_width);
        }
    }
    catch (...) {
        min_width_ = backup_min_width;
        max_width_ = backup_max_width;
        throw;
    }
}


float WindowGeometryFacet::MaxWidth() const noexcept {
    if (max_width_) {
        return *max_width_;
    }
    return ToDIPs(static_cast<float>(GetSystemMetrics(SM_CXMAXTRACK)), DPI());
}


void WindowGeometryFacet::SetMaxWidth(float max_width) {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::Destroying ||
        handle_state == WindowHandleState::Destroyed) {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    auto backup_min_width = min_width_;
    auto backup_max_width = max_width_;

    max_width_ = max_width;

    if (MinWidth() > max_width) {
        min_width_ = max_width;
    }

    try {
        if (Width() > max_width) {
            SetWidth(max_width);
        }
    }
    catch (...) {
        min_width_ = backup_min_width;
        max_width_ = backup_max_width;
        throw;
    }
}


float WindowGeometryFacet::MinHeight() const noexcept {
    if (min_height_) {
        return *min_height_;
    }
    return ToDIPs(static_cast<float>(GetSystemMetrics(SM_CYMINTRACK)), DPI());
}


void WindowGeometryFacet::SetMinHeight(float min_height) {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::Destroying ||
        handle_state == WindowHandleState::Destroyed) {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    auto backup_min_height = min_height_;
    auto backup_max_height = max_height_;

    min_height_ = min_height;

    if (MaxHeight() < min_height) {
        max_height_ = min_height;
    }

    try {
        if (Height() < min_height) {
            SetHeight(min_height);
        }
    }
    catch (...) {
        min_height_ = backup_min_height;
        max_height_ = backup_max_height;
        throw;
    }
}


float WindowGeometryFacet::MaxHeight() const noexcept {
    if (max_height_) {
        return *max_height_;
    }
    return ToDIPs(static_cast<float>(GetSystemMetrics(SM_CYMAXTRACK)), DPI());
}


void WindowGeometryFacet::SetMaxHeight(float max_height) {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::Destroying ||
        handle_state == WindowHandleState::Destroyed) {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    auto backup_min_height = min_height_;
    auto backup_max_height = max_height_;

    max_height_ = max_height;

    if (MinHeight() > max_height) {
        min_height_ = max_height;
    }

    try {
        if (Height() > max_height) {
            SetHeight(max_height);
        }
    }
    catch (...) {
        min_height_ = backup_min_height;
        max_height_ = backup_max_height;
        throw;
    }
}


zaf::Size WindowGeometryFacet::MinSize() const noexcept {
    return zaf::Size{ MinWidth(), MinHeight() };
}


void WindowGeometryFacet::SetMinSize(const zaf::Size& size) {
    SetMinWidth(size.width);
    SetMinHeight(size.height);
}


zaf::Size WindowGeometryFacet::MaxSize() const noexcept {
    return zaf::Size{ MaxWidth(), MaxHeight() };
}


void WindowGeometryFacet::SetMaxSize(const zaf::Size& size) {
    SetMaxWidth(size.width);
    SetMaxHeight(size.height);
}


zaf::Rect WindowGeometryFacet::ContentRect() const noexcept {
    return zaf::Rect{ Point{}, ContentSize() };
}


zaf::Size WindowGeometryFacet::ContentSize() const noexcept {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::NotCreated) {

        const auto& not_created_state_data = window_.lifecycle_facet_->NotCreatedStateData();
        if (window_.UseCustomFrame()) {
            return not_created_state_data.size;
        }

        auto frame = GetWindowFrame(
            this->DPI(),
            not_created_state_data.basic_style,
            not_created_state_data.extended_style);

        auto result = not_created_state_data.size;
        result.width = (std::max)(result.width - frame.Width(), 0.f);
        result.height = (std::max)(result.height - frame.Height(), 0.f);
        return result;
    }

    if (handle_state == WindowHandleState::Creating ||
        handle_state == WindowHandleState::Created ||
        handle_state == WindowHandleState::Destroying) {

        RECT client_rect{};
        ::GetClientRect(window_.Handle(), &client_rect);
        return ToDIPs(Rect::FromRECT(client_rect).size, DPI());
    }

    return {};
}


void WindowGeometryFacet::SetContentSize(const zaf::Size& size) {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::Destroying ||
        handle_state == WindowHandleState::Destroyed) {

        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    zaf::Size window_size;

    if (window_.UseCustomFrame()) {
        window_size = size;
    }
    else {

        if (handle_state == WindowHandleState::NotCreated) {

            auto& not_created_state_data = window_.lifecycle_facet_->NotCreatedStateData();
            window_size = AdjustContentSizeToWindowSize(
                size,
                this->DPI(),
                not_created_state_data.basic_style,
                not_created_state_data.extended_style);
        }
        else {

            window_size = AdjustContentSizeToWindowSize(
                size,
                this->DPI(),
                internal::WindowBasicStyle::FromHandle(window_.Handle()),
                internal::WindowExtendedStyle::FromHandle(window_.Handle()));
        }
    }

    SetSize(window_size);
}


float WindowGeometryFacet::ContentWidth() const noexcept {
    return ContentSize().width;
}


void WindowGeometryFacet::SetContentWidth(float width) {
    zaf::Size new_size = ContentSize();
    new_size.width = width;
    SetContentSize(new_size);
}


float WindowGeometryFacet::ContentHeight() const noexcept {
    return ContentSize().height;
}


void WindowGeometryFacet::SetContentHeight(float height) {
    zaf::Size new_size = ContentSize();
    new_size.height = height;
    SetContentSize(new_size);
}


void WindowGeometryFacet::HandleWMGETMINMAXINFO(const Message& message) {

    auto dpi = this->DPI();
    auto min_max_info = reinterpret_cast<MINMAXINFO*>(message.LParam());
    min_max_info->ptMinTrackSize.x = static_cast<LONG>(FromDIPs(MinWidth(), dpi));
    min_max_info->ptMinTrackSize.y = static_cast<LONG>(FromDIPs(MinHeight(), dpi));
    min_max_info->ptMaxTrackSize.x = static_cast<LONG>(FromDIPs(MaxWidth(), dpi));
    min_max_info->ptMaxTrackSize.y = static_cast<LONG>(FromDIPs(MaxHeight(), dpi));
}


void WindowGeometryFacet::HandleWMSIZE(const Message& message) {

    zaf::Size new_size{
        static_cast<float>(LOWORD(message.LParam())),
        static_cast<float>(HIWORD(message.LParam()))
    };

    auto& state_data = window_.lifecycle_facet_->HandleStateData();
    if (state_data.renderer) {
        state_data.renderer.Resize(new_size);
    }

    auto& handle_state_data = window_.lifecycle_facet_->HandleStateData();
    if (!handle_state_data.is_handling_dpi_changed) {

        zaf::Rect root_control_rect{ Point(), ToDIPs(new_size, DPI()) };
        window_.RootControl()->SetRect(root_control_rect);

        RaiseSizeChangedEvent();
    }
}


void WindowGeometryFacet::RaiseSizeChangedEvent() {
    window_.OnSizeChanged(WindowSizeChangedInfo{ window_.shared_from_this() });
}


void WindowGeometryFacet::HandleWMDPICHANGED(const Message& message) {

    {
        auto& handle_state_data = window_.lifecycle_facet_->HandleStateData();
        auto auto_reset = MakeAutoReset(handle_state_data.is_handling_dpi_changed, true);

        // The LParam contains a pointer to a RECT with the suggested window position and size
        auto suggested_rect = reinterpret_cast<const RECT*>(message.LParam());

        SetWindowPos(
            window_.Handle(),
            nullptr,
            suggested_rect->left,
            suggested_rect->top,
            suggested_rect->right - suggested_rect->left,
            suggested_rect->bottom - suggested_rect->top,
            SWP_NOZORDER | SWP_NOACTIVATE);
    }

    window_.OnDPIChanged(DPIChangedInfo{ window_.shared_from_this() });
}


Frame WindowGeometryFacet::GetWindowFrame(
    float dpi,
    const internal::WindowBasicStyle& basic_style,
    const internal::WindowExtendedStyle& extend_style) noexcept {

    auto window_rect = AdjustContentRectToWindowRect(zaf::Rect{}, dpi, basic_style, extend_style);
    return Frame{
        std::abs(window_rect.Left()),
        std::abs(window_rect.Top()),
        window_rect.Right(),
        window_rect.Bottom()
    };
}


zaf::Size WindowGeometryFacet::AdjustContentSizeToWindowSize(
    const zaf::Size& content_size,
    float dpi,
    const internal::WindowBasicStyle& basic_style,
    const internal::WindowExtendedStyle& extend_style) noexcept {

    zaf::Rect content_rect{ Point{}, content_size };
    auto window_rect = AdjustContentRectToWindowRect(
        content_rect,
        dpi,
        basic_style,
        extend_style);

    return window_rect.size;
}


zaf::Rect WindowGeometryFacet::AdjustContentRectToWindowRect(
    const zaf::Rect& content_rect, 
    float dpi,
    const internal::WindowBasicStyle& basic_style, 
    const internal::WindowExtendedStyle& extend_style) noexcept {

    auto snapped_content_rect = SnapAndTransformToPixels(content_rect, dpi);
    auto adjusted_rect = snapped_content_rect.ToRECT();

    AdjustWindowRectExForDpi(
        &adjusted_rect,
        basic_style.Value(),
        FALSE,
        extend_style.Value(),
        static_cast<UINT>(dpi));

    auto result = zaf::Rect::FromRECT(adjusted_rect);
    result = ToDIPs(result, dpi);
    return result;
}


bool WindowGeometryFacet::IsSizingOrMoving() const noexcept {
    return window_.lifecycle_facet_->HandleStateData().is_sizing_or_moving;
}


rx::Single<None> WindowGeometryFacet::WhenNotSizingOrMoving() const {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::Creating ||
        handle_state == WindowHandleState::Created ||
        handle_state == WindowHandleState::Destroying) {

        auto& state_data = window_.lifecycle_facet_->HandleStateData();
        if (!state_data.is_sizing_or_moving) {
            return rx::Single<None>::Just({});
        }

        if (!state_data.exit_sizing_or_moving_subject) {
            state_data.exit_sizing_or_moving_subject.emplace();
        }

        return state_data.exit_sizing_or_moving_subject->AsSingle();
    }

    return rx::Single<None>::Just({});
}


void WindowGeometryFacet::HandleWMENTERSIZEMOVE() noexcept {
    window_.lifecycle_facet_->HandleStateData().is_sizing_or_moving = true;
}


void WindowGeometryFacet::HandleWMEXITSIZEMOVE() {

    auto& state_data = window_.lifecycle_facet_->HandleStateData();
    state_data.is_sizing_or_moving = false;

    if (state_data.exit_sizing_or_moving_subject) {
        auto observer = state_data.exit_sizing_or_moving_subject->AsObserver();
        observer.OnSuccess({});
        state_data.exit_sizing_or_moving_subject.reset();
    }
}


Point WindowGeometryFacet::TransformToScreen(const Point& position) const noexcept {
    auto result = GetContentOriginInScreen();
    result.AddOffset(position);
    return result;
}


Point WindowGeometryFacet::TransformFromScreen(const Point& position) const noexcept {
    auto result = position;
    auto content_origin = GetContentOriginInScreen();
    result.SubtractOffset(content_origin);
    return result;
}


zaf::Rect WindowGeometryFacet::TransformToScreen(const zaf::Rect& rect) const noexcept {
    return zaf::Rect{ TransformToScreen(rect.position), rect.size };
}


zaf::Rect WindowGeometryFacet::TransformFromScreen(const zaf::Rect& rect) const noexcept {
    return zaf::Rect{ TransformFromScreen(rect.position), rect.size };
}


Point WindowGeometryFacet::GetContentOriginInScreen() const noexcept {

    auto handle_state = window_.HandleState();
    if (handle_state == WindowHandleState::NotCreated) {

        auto& not_create_state_data = window_.lifecycle_facet_->NotCreatedStateData();

        auto screen = this->Screen();
        auto initial_rect = ResolveInitialRect(*screen, window_.Owner(), not_create_state_data);

        auto window_frame = GetWindowFrame(
            screen->DPI(),
            not_create_state_data.basic_style,
            not_create_state_data.extended_style);

        auto result = initial_rect.position;
        result.AddOffset(Point{ window_frame.left, window_frame.top });
        return result;
    }
    else if (handle_state == WindowHandleState::Creating ||
             handle_state == WindowHandleState::Created ||
             handle_state == WindowHandleState::Destroying) {

        POINT point{};
        ClientToScreen(window_.Handle(), &point);

        auto result = this->Screen()->TransformFromGlobal(Point::FromPOINT(point));
        return result;
    }
    else {
        return {};
    }
}

}