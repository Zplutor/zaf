#include <zaf/window/internal/window_facets/window_keyboard_facet.h>
#include <zaf/control/event/ime_event_infos.h>
#include <zaf/internal/control/keyboard_event_routing.h>
#include <zaf/window/internal/window_facets/window_focus_facet.h>
#include <zaf/window/window.h>

namespace zaf::internal {

WindowKeyboardFacet::WindowKeyboardFacet(Window& window) noexcept : window_(window) {

}


bool WindowKeyboardFacet::PreprocessMessage(const KeyMessage& message) {

    if (window_.focus_facet_->TryToPreprocessTabKeyMessage(message)) {
        return true;
    }

    if (TryToPreprocessInspectorShortcutMessage(message)) {
        return true;
    }

    return false;
}


bool WindowKeyboardFacet::TryToPreprocessInspectorShortcutMessage(const KeyMessage& message) {
#ifndef NDEBUG
    if (message.ID() != WM_KEYDOWN) {
        return false;
    }

    if (message.Key() != Key::F12) {
        return false;
    }

    if ((GetKeyState(VK_CONTROL) >> 15) == 0) {
        return false;
    }

    window_.ShowInspector();
    return true;
#else
    return false;
#endif
}


bool WindowKeyboardFacet::HandleKeyboardMessage(const Message& message) {

    auto focused_control = window_.FocusedControl();
    if (focused_control) {
        return RouteKeyboardEvent(focused_control, message);
    }
    return false;
}


bool WindowKeyboardFacet::HandleIMEMessage(const Message& message) {

    auto focused_control = window_.FocusedControl();
    if (!focused_control) {
        return false;
    }

    switch (message.ID()) {
    case WM_IME_STARTCOMPOSITION: {
        IMEStartCompositionInfo event_info{ 
            std::make_shared<internal::IMEEventSharedState>(focused_control, message), 
            focused_control 
        };
        focused_control->OnIMEStartComposition(event_info);
        return event_info.IsHandled();
    }
    case WM_IME_ENDCOMPOSITION: {
        IMEEndCompositionInfo event_info{ 
            std::make_shared<internal::IMEEventSharedState>(focused_control, message), 
            focused_control 
        };
        focused_control->OnIMEEndComposition(event_info);
        return event_info.IsHandled();
    }
    case WM_IME_COMPOSITION: {
        IMECompositionInfo event_info{ 
            std::make_shared<internal::IMEEventSharedState>(focused_control, message), 
            focused_control 
        };
        focused_control->OnIMEComposition(event_info);
        return event_info.IsHandled();
    }
    default:
        return false;
    }
}

}