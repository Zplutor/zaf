#include <gtest/gtest.h>
#include <Windows.h>
#include <zaf/control/control.h>
#include <zaf/core/object/creation.h>
#include <zaf/window/window.h>
#include "utility/test_window.h"

TEST(ControlTest, CaptureMouse_NotInWindow) {

    auto control = zaf::Create<zaf::Control>();
    ASSERT_FALSE(control->HasMouseCapture());
    ASSERT_FALSE(control->CaptureMouse());
    ASSERT_FALSE(control->HasMouseCapture());
}


TEST(ControlTest, CaptureMouse_WindowHandleNotCreated) {

    auto window = zaf::Create<zaf::Window>();
    auto control = zaf::Create<zaf::Control>();
    window->RootControl()->AddChild(control);

    ASSERT_FALSE(control->CaptureMouse());
    ASSERT_FALSE(control->HasMouseCapture());
    ASSERT_EQ(window->MouseCaptureControl(), nullptr);
}


TEST(ControlTest, CaptureMouse) {

    TestWithWindow([](zaf::Window& window) {

        auto control = zaf::Create<zaf::Control>();
        window.RootControl()->AddChild(control);

        ASSERT_TRUE(control->CaptureMouse());
        ASSERT_TRUE(control->HasMouseCapture());
        ASSERT_EQ(window.MouseCaptureControl(), control);
        ASSERT_EQ(window.Handle(), GetCapture());

        // Capture mouse again is a non-op.
        ASSERT_TRUE(control->CaptureMouse());
    });
}


TEST(ControlTest, CaptureMouse_TakeOverCapture) {

    TestWithWindow([](zaf::Window& window) {

        auto control = zaf::Create<zaf::Control>();
        auto other_control = zaf::Create<zaf::Control>();
        window.RootControl()->AddChildren({ control, other_control });

        control->CaptureMouse();

        ASSERT_TRUE(other_control->CaptureMouse());
        ASSERT_TRUE(other_control->HasMouseCapture());
        ASSERT_EQ(window.MouseCaptureControl(), other_control);
    });
}


TEST(ControlTest, MouseCapturedEvent_NotInWindow) {

    auto control = zaf::Create<zaf::Control>();
    bool is_event_called{};
    auto sub = control->MouseCapturedEvent().Subscribe(
        [&](const zaf::MouseCapturedInfo&) {
            is_event_called = true;
        });
    control->CaptureMouse();
    ASSERT_FALSE(is_event_called);
}


TEST(ControlTest, MouseCapturedEvent_WindowHandleNotCreated) {

    auto window = zaf::Create<zaf::Window>();
    auto control = zaf::Create<zaf::Control>();
    window->RootControl()->AddChild(control);

    bool is_event_called{};
    auto sub = control->MouseCapturedEvent().Subscribe(
        [&](const zaf::MouseCapturedInfo&) {
            is_event_called = true;
        });
    control->CaptureMouse();
    ASSERT_FALSE(is_event_called);
}


TEST(ControlTest, MouseCapturedEvent) {

    TestWithWindow([](zaf::Window& window) {

        auto control = zaf::Create<zaf::Control>();
        window.RootControl()->AddChild(control);

        int event_called_count{};
        bool is_event_info_ok{};

        auto sub = control->MouseCapturedEvent().Subscribe(
            [&](const zaf::MouseCapturedInfo& event_info) {
                is_event_info_ok = event_info.Source() == control;
                ++event_called_count;
            });

        control->CaptureMouse();
        ASSERT_EQ(event_called_count, 1);
        ASSERT_TRUE(is_event_info_ok);

        // Capture mouse again wouldn't raise the event.
        control->CaptureMouse();
        ASSERT_EQ(event_called_count, 1);
    });
}


TEST(ControlTest, MouseCapturedEvent_TakeOverCapture) {

    TestWithWindow([](zaf::Window& window) {

        auto control = zaf::Create<zaf::Control>();
        auto other_control = zaf::Create<zaf::Control>();
        window.RootControl()->AddChildren({ control, other_control });

        control->CaptureMouse();

        int event_called_count{};
        bool is_event_info_ok{};
        auto sub = other_control->MouseCapturedEvent().Subscribe(
            [&](const zaf::MouseCapturedInfo& event_info) {
                is_event_info_ok = event_info.Source() == other_control;
                ++event_called_count;
            });

        other_control->CaptureMouse();
        ASSERT_EQ(event_called_count, 1);
        ASSERT_TRUE(is_event_info_ok);
    });
}


TEST(ControlTest, ReleaseMouse_NotInWindow) {
    auto control = zaf::Create<zaf::Control>();
    ASSERT_NO_THROW(control->ReleaseMouse());
}


TEST(ControlTest, ReleaseMouse_WindowHandleNotCreated) {

    auto window = zaf::Create<zaf::Window>();
    auto control = zaf::Create<zaf::Control>();
    window->RootControl()->AddChild(control);
    ASSERT_NO_THROW(control->ReleaseMouse());
}


TEST(ControlTest, ReleaseMouse_NotCaptured) {

    TestWithWindow([](zaf::Window& window) {

        auto control = zaf::Create<zaf::Control>();
        window.RootControl()->AddChild(control);

        ASSERT_NO_THROW(control->ReleaseMouse());
        ASSERT_FALSE(control->HasMouseCapture());
    });
}


TEST(ControlTest, ReleaseMouse) {

    TestWithWindow([](zaf::Window& window) {

        auto control = zaf::Create<zaf::Control>();
        window.RootControl()->AddChild(control);
        
        control->CaptureMouse();

        control->ReleaseMouse();
        ASSERT_FALSE(control->HasMouseCapture());
        ASSERT_EQ(window.MouseCaptureControl(), nullptr);
        ASSERT_EQ(::GetCapture(), nullptr);
    });
}


TEST(ControlTest, MouseReleasedEvent_NotInWindow) {

    auto control = zaf::Create<zaf::Control>();

    bool is_event_called{};
    auto sub = control->MouseReleasedEvent().Subscribe(
        [&](const zaf::MouseReleasedInfo&) {
            is_event_called = true;
        });

    control->ReleaseMouse();
    ASSERT_FALSE(is_event_called);
}


TEST(ControlTest, MouseReleasedEvent_WindowHandleNotCreated) {

    auto window = zaf::Create<zaf::Window>();
    auto control = zaf::Create<zaf::Control>();
    window->RootControl()->AddChild(control);

    bool is_event_called{};
    auto sub = control->MouseReleasedEvent().Subscribe(
        [&](const zaf::MouseReleasedInfo&) {
            is_event_called = true;
        });

    control->ReleaseMouse();
    ASSERT_FALSE(is_event_called);
}


TEST(ControlTest, MouseReleasedEvent_NotCaptured) {

    TestWithWindow([](zaf::Window& window) {

        auto control = zaf::Create<zaf::Control>();
        window.RootControl()->AddChild(control);

        bool is_event_called{};
        auto sub = control->MouseReleasedEvent().Subscribe(
            [&](const zaf::MouseReleasedInfo&) {
                is_event_called = true;
            });
        control->ReleaseMouse();
        ASSERT_FALSE(is_event_called);
    });
}


TEST(ControlTest, MouseReleasedEvent) {

    TestWithWindow([](zaf::Window& window) {

        auto control = zaf::Create<zaf::Control>();
        window.RootControl()->AddChild(control);

        control->CaptureMouse();

        int event_called_count{};
        bool is_event_info_ok{};
        auto sub = control->MouseReleasedEvent().Subscribe(
            [&](const zaf::MouseReleasedInfo& event_info) {
                is_event_info_ok = event_info.Source() == control;
                ++event_called_count;
            });

        control->ReleaseMouse();
        ASSERT_EQ(event_called_count, 1);
        ASSERT_TRUE(is_event_info_ok);

        // Release mouse again wouldn't raise the event.
        control->ReleaseMouse();
        ASSERT_EQ(event_called_count, 1);
    });
}


TEST(ControlTest, MouseReleasedEvent_TakeOverCapture) {

    TestWithWindow([](zaf::Window& window) {

        auto control = zaf::Create<zaf::Control>();
        auto other_control = zaf::Create<zaf::Control>();
        window.RootControl()->AddChildren({ control, other_control });

        control->CaptureMouse();

        int event_called_count{};
        bool is_event_info_ok{};
        auto sub = control->MouseReleasedEvent().Subscribe(
            [&](const zaf::MouseReleasedInfo& event_info) {
                is_event_info_ok = event_info.Source() == control;
                ++event_called_count;
            });

        other_control->CaptureMouse();
        ASSERT_EQ(event_called_count, 1);
        ASSERT_TRUE(is_event_info_ok);
    });
}


TEST(ControlTest, MouseCaptureControlChangedEvent) {

    TestWithWindow([](zaf::Window& window) {

        auto control1 = zaf::Create<zaf::Control>();
        auto control2 = zaf::Create<zaf::Control>();
        window.RootControl()->AddChildren({ control1, control2 });

        // New mouse capture.
        {
            int event_called_count{};
            bool is_event_info_ok{};
            auto sub = window.MouseCaptureControlChangedEvent().Subscribe(
                [&](const zaf::MouseCaptureControlChangedInfo& event_info) {
                    is_event_info_ok =
                        event_info.Source().get() == &window &&
                        event_info.PreviousControl() == nullptr;
                    ++event_called_count;
                });
            control1->CaptureMouse();
            ASSERT_EQ(event_called_count, 1);
            ASSERT_TRUE(is_event_info_ok);

            // Capture mouse again wouldn't raise the event.
            control1->CaptureMouse();
            ASSERT_EQ(event_called_count, 1);
        }
        
        // Take over mouse capture.
        {
            int event_called_count{};
            bool is_event_info_ok{};
            auto sub = window.MouseCaptureControlChangedEvent().Subscribe(
                [&](const zaf::MouseCaptureControlChangedInfo& event_info) {
                    is_event_info_ok =
                        event_info.Source().get() == &window &&
                        event_info.PreviousControl() == control1;
                    ++event_called_count;
                });
            control2->CaptureMouse();
            ASSERT_EQ(event_called_count, 1);
            ASSERT_TRUE(is_event_info_ok);
        }

        // Release mouse capture.
        {
            int event_called_count{};
            bool is_event_info_ok{};
            auto sub = window.MouseCaptureControlChangedEvent().Subscribe(
                [&](const zaf::MouseCaptureControlChangedInfo& event_info) {
                    is_event_info_ok =
                        event_info.Source().get() == &window &&
                        event_info.PreviousControl() == control2;
                    ++event_called_count;
                });
            control2->ReleaseMouse();
            ASSERT_EQ(event_called_count, 1);
            ASSERT_TRUE(is_event_info_ok);
        }
    });
}


TEST(ControlTest, LossMouseCapturePassively) {

    TestWithWindow([](zaf::Window& window) {

        auto control = zaf::Create<zaf::Control>();
        window.RootControl()->AddChild(control);

        control->CaptureMouse();

        int mouse_released_event_count{};
        bool is_mouse_released_event_info_ok{};
        auto sub = control->MouseReleasedEvent().Subscribe(
            [&](const zaf::MouseReleasedInfo& event_info) {
                is_mouse_released_event_info_ok = event_info.Source() == control;
                ++mouse_released_event_count;
            });

        int mouse_capture_control_changed_event_count{};
        bool is_mouse_capture_control_changed_event_info_ok{};
        auto sub2 = window.MouseCaptureControlChangedEvent().Subscribe(
            [&](const zaf::MouseCaptureControlChangedInfo& event_info) {
                is_mouse_capture_control_changed_event_info_ok =
                    event_info.Source().get() == &window &&
                    event_info.PreviousControl() == control;
                ++mouse_capture_control_changed_event_count;
            });

        ::ReleaseCapture();

        ASSERT_FALSE(control->HasMouseCapture());
        ASSERT_EQ(window.MouseCaptureControl(), nullptr);
        ASSERT_EQ(::GetCapture(), nullptr);
        ASSERT_EQ(mouse_released_event_count, 1);
        ASSERT_TRUE(is_mouse_released_event_info_ok);
        ASSERT_EQ(mouse_capture_control_changed_event_count, 1);
        ASSERT_TRUE(is_mouse_capture_control_changed_event_info_ok);
    });
}


TEST(ControlTest, MouseCaptureEventsOrder_NewCapture) {

    TestWithWindow([](zaf::Window& window) {

        auto control = zaf::Create<zaf::Control>();
        window.RootControl()->AddChild(control);

        int order_seed{};
        int mouse_captured_event_order{};
        int mouse_capture_control_changed_event_order{};

        auto sub = control->MouseCapturedEvent().Subscribe(
            [&](const zaf::MouseCapturedInfo&) {
                mouse_captured_event_order = ++order_seed;
            });

        auto sub2 = window.MouseCaptureControlChangedEvent().Subscribe(
            [&](const zaf::MouseCaptureControlChangedInfo&) {
                mouse_capture_control_changed_event_order = ++order_seed;
            });

        control->CaptureMouse();

        ASSERT_EQ(mouse_captured_event_order, 1);
        ASSERT_EQ(mouse_capture_control_changed_event_order, 2);
    });
}


TEST(ControlTest, MouseCaptureEventsOrder_TakeOverCapture) {

    TestWithWindow([](zaf::Window& window) {

        auto control1 = zaf::Create<zaf::Control>();
        auto control2 = zaf::Create<zaf::Control>();
        window.RootControl()->AddChildren({ control1, control2 });

        control1->CaptureMouse();

        int order_seed{};
        int mouse_released_event_order{};
        int mouse_capture_control_changed_event_order{};
        int mouse_captured_event_order{};

        zaf::rx::DisposeBag subs;
        subs += control1->MouseReleasedEvent().Subscribe(
            [&](const zaf::MouseReleasedInfo&) {
                mouse_released_event_order = ++order_seed;
            });

        subs += control2->MouseCapturedEvent().Subscribe(
            [&](const zaf::MouseCapturedInfo&) {
                mouse_captured_event_order = ++order_seed;
            });

        subs += window.MouseCaptureControlChangedEvent().Subscribe(
            [&](const zaf::MouseCaptureControlChangedInfo&) {
                mouse_capture_control_changed_event_order = ++order_seed;
            });

        control2->CaptureMouse();

        ASSERT_EQ(mouse_released_event_order, 1);
        ASSERT_EQ(mouse_captured_event_order, 2);
        ASSERT_EQ(mouse_capture_control_changed_event_order, 3);
    });
}


TEST(ControlTest, MouseCaptureEventsOrder_ReleaseCapture) {

    TestWithWindow([](zaf::Window& window) {

        auto control = zaf::Create<zaf::Control>();
        window.RootControl()->AddChild(control);

        control->CaptureMouse();

        int order_seed{};
        int mouse_released_event_order{};
        int mouse_capture_control_changed_event_order{};
        auto sub = control->MouseReleasedEvent().Subscribe(
            [&](const zaf::MouseReleasedInfo&) {
                mouse_released_event_order = ++order_seed;
            });

        auto sub2 = window.MouseCaptureControlChangedEvent().Subscribe(
            [&](const zaf::MouseCaptureControlChangedInfo&) {
                mouse_capture_control_changed_event_order = ++order_seed;
            });

        control->ReleaseMouse();
        ASSERT_EQ(mouse_released_event_order, 1);
        ASSERT_EQ(mouse_capture_control_changed_event_order, 2);
    });
}


TEST(ControlTest, MouseCaptureEventsOrder_LoseCapturePassively) {

    TestWithWindow([](zaf::Window& window) {

        auto control = zaf::Create<zaf::Control>();
        window.RootControl()->AddChild(control);

        control->CaptureMouse();

        int order_seed{};
        int mouse_released_event_order{};
        int mouse_capture_control_changed_event_order{};

        auto sub = control->MouseReleasedEvent().Subscribe(
            [&](const zaf::MouseReleasedInfo&) {
                mouse_released_event_order = ++order_seed;
            });

        auto sub2 = window.MouseCaptureControlChangedEvent().Subscribe(
            [&](const zaf::MouseCaptureControlChangedInfo&) {
                mouse_capture_control_changed_event_order = ++order_seed;
            });

        ::ReleaseCapture();

        ASSERT_EQ(mouse_released_event_order, 1);
        ASSERT_EQ(mouse_capture_control_changed_event_order, 2);
    });
}