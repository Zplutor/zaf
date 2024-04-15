#include <zaf/internal/control/control_event_invoker.h>
#include <zaf/base/error/check.h>
#include <zaf/control/control.h>

namespace zaf::internal {
namespace {

template<typename E>
using ControlEventFunction = void(Control::*)(const E&);

template<typename E>
MouseEventInvoker MakeMouseEventInvoker(ControlEventFunction<E> event_function) {

    auto invoker = [](
        ControlEventFunction<E> event_function,
        const std::shared_ptr<MouseEventSharedState>& event_state,
        const std::shared_ptr<Control>& sender,
        const Point& position_at_sender) {
    
        E event_info{ event_state, sender, position_at_sender };
        (sender.get()->*event_function)(event_info);
    };

    return std::bind(
        invoker,
        event_function, 
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3);
}


template<typename E>
KeyboardEventInvoker MakeKeyboardEventInvoker(ControlEventFunction<E> event_function) {

    auto invoker = [](
        ControlEventFunction<E> event_function,
        const std::shared_ptr<KeyboardEventSharedState>& event_state,
        const std::shared_ptr<Control>& sender) {
    
        E event_info{ event_state, sender };
        (sender.get()->*event_function)(event_info);
    };

    return std::bind(invoker, event_function, std::placeholders::_1, std::placeholders::_2);
}

}


const MouseEventInvoker& ControlEventInvokerBinder::GetPreMouseEventInvoker(UINT message_id) {
    return GetMouseEventBinding(message_id).pre_event_invoker;
}


const MouseEventInvoker& ControlEventInvokerBinder::GetMouseEventInvoker(UINT message_id) {
    return GetMouseEventBinding(message_id).event_invoker;
}


const KeyboardEventInvoker& ControlEventInvokerBinder::GetPreKeyboardEventInvoker(
    UINT message_id) {

    return GetKeyboardEventBinding(message_id).pre_event_invoker;
}


const KeyboardEventInvoker& ControlEventInvokerBinder::GetKeyboardEventInvoker(UINT message_id) {
    return GetKeyboardEventBinding(message_id).event_invoker;
}


const ControlEventInvokerBinder::MouseEventBinding& 
    ControlEventInvokerBinder::GetMouseEventBinding(UINT message_id) {

    switch (message_id) {
        case WM_NCMOUSEMOVE:
        case WM_MOUSEMOVE: {
            static MouseEventBinding binding{
                MakeMouseEventInvoker<PreMouseMoveInfo>(&Control::OnPreMouseMove),
                MakeMouseEventInvoker<MouseMoveInfo>(&Control::OnMouseMove),
            };
            return binding;
        }

        case WM_LBUTTONDOWN:
        case WM_NCLBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN: {
            static MouseEventBinding binding{
                MakeMouseEventInvoker<PreMouseDownInfo>(&Control::OnPreMouseDown),
                MakeMouseEventInvoker<MouseDownInfo>(&Control::OnMouseDown),
            };
            return binding;
        }

        case WM_LBUTTONUP:
        case WM_NCLBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            static MouseEventBinding binding{
                MakeMouseEventInvoker<PreMouseUpInfo>(&Control::OnPreMouseUp),
                MakeMouseEventInvoker<MouseUpInfo>(&Control::OnMouseUp),
            };
            return binding;
        }

        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL: {
            static MouseEventBinding binding{
                MakeMouseEventInvoker<PreMouseWheelInfo>(&Control::OnPreMouseWheel),
                MakeMouseEventInvoker<MouseWheelInfo>(&Control::OnMouseWheel),
            };
            return binding;
        }

        default: 
            ZAF_NOT_REACHED();
    }
}


const ControlEventInvokerBinder::KeyboardEventBinding& 
    ControlEventInvokerBinder::GetKeyboardEventBinding(UINT message_id) {

    switch (message_id) {
        case WM_KEYDOWN: {
            static KeyboardEventBinding binding{
                MakeKeyboardEventInvoker<PreKeyDownInfo>(&Control::OnPreKeyDown),
                MakeKeyboardEventInvoker<KeyDownInfo>(&Control::OnKeyDown),
            };
            return binding;
        }

        case WM_KEYUP: {
            static KeyboardEventBinding binding{
                MakeKeyboardEventInvoker<PreKeyUpInfo>(&Control::OnPreKeyUp),
                MakeKeyboardEventInvoker<KeyUpInfo>(&Control::OnKeyUp),
            };
            return binding;
        }

        case WM_CHAR: {
            static KeyboardEventBinding binding{
                MakeKeyboardEventInvoker<PreCharInputInfo>(&Control::OnPreCharInput),
                MakeKeyboardEventInvoker<CharInputInfo>(&Control::OnCharInput),
            };
            return binding;
        }

        case WM_SYSKEYDOWN: {
            static KeyboardEventBinding binding{
                MakeKeyboardEventInvoker<PreSysKeyDownInfo>(&Control::OnPreSysKeyDown),
                MakeKeyboardEventInvoker<SysKeyDownInfo>(&Control::OnSysKeyDown),
            };
            return binding;
        }

        case WM_SYSKEYUP: {
            static KeyboardEventBinding binding{
                MakeKeyboardEventInvoker<PreSysKeyUpInfo>(&Control::OnPreSysKeyUp),
                MakeKeyboardEventInvoker<SysKeyUpInfo>(&Control::OnSysKeyUp),
            };
            return binding;
        }

        case WM_SYSCHAR: {
            static KeyboardEventBinding binding{
                MakeKeyboardEventInvoker<PreSysCharInputInfo>(&Control::OnPreSysCharInput),
                MakeKeyboardEventInvoker<SysCharInputInfo>(&Control::OnSysCharInput),
            };
            return binding;
        }

        default:
            ZAF_NOT_REACHED();
    }
}

}