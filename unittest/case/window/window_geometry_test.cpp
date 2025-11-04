#include <gtest/gtest.h>
#include <zaf/graphic/dpi.h>
#include <zaf/graphic/pixel_snapping.h>
#include <zaf/window/invalid_handle_state_error.h>
#include <zaf/window/window.h>
#include "window_test.h"

namespace zaf::testing {
namespace {

static const zaf::Size DefaultWindowSize{ 640, 480 };

}

TEST_F(WindowTest, DefaultRect_NotCreated_NoOwner) {

    auto window = zaf::Create<zaf::Window>();
    auto rect = window->Rect();
    ASSERT_EQ(rect.size, DefaultWindowSize);
    
    auto dpi = window->GetDPI();
    float screen_width = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CXSCREEN)), dpi);
    float screen_height = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CYSCREEN)), dpi);

    zaf::Point expected_position{
        (screen_width - DefaultWindowSize.width) / 2,
        (screen_height - DefaultWindowSize.height) / 2
    };
    ASSERT_EQ(rect.position, expected_position);
}


TEST_F(WindowTest, DefaultRect_NotCreated_Owner) {

    auto owner = zaf::Create<zaf::Window>();
    owner->SetRect(zaf::Rect{ 100, 100, 800, 600 });

    auto window = zaf::Create<zaf::Window>();
    window->SetOwner(owner);

    auto rect = window->Rect();
    ASSERT_EQ(rect.size, DefaultWindowSize);

    zaf::Point expected_position{
        100 + (800 - DefaultWindowSize.width) / 2,
        100 + (600 - DefaultWindowSize.height) / 2
    };
    ASSERT_EQ(rect.position, expected_position);
}


TEST_F(WindowTest, DefaultRect_Creating_NoOwner) {
    //TODO
}


TEST_F(WindowTest, DefaultRect_Creating_Owner) {
    //TODO
}


TEST_F(WindowTest, DefaultRect_Created_NoOwner) {
    
    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();

    auto rect = window->Rect();
    ASSERT_EQ(rect.size, DefaultWindowSize);

    auto dpi = window->GetDPI();
    float screen_width = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CXSCREEN)), dpi);
    float screen_height = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CYSCREEN)), dpi);
    zaf::Point expected_position{
        (screen_width - DefaultWindowSize.width) / 2,
        (screen_height - DefaultWindowSize.height) / 2
    };
    ASSERT_EQ(rect.position, expected_position);
    window->Destroy();
}


TEST_F(WindowTest, DefaultRect_Created_Owner) {

    auto owner = zaf::Create<zaf::Window>();
    owner->SetRect(zaf::Rect{ 100, 100, 800, 600 });
    auto owner_holder = owner->CreateHandle();

    auto window = zaf::Create<zaf::Window>();
    window->SetOwner(owner);
    auto holder = window->CreateHandle();

    auto rect = window->Rect();
    ASSERT_EQ(rect.size, DefaultWindowSize);
    zaf::Point expected_position{
        100 + (800 - DefaultWindowSize.width) / 2,
        100 + (600 - DefaultWindowSize.height) / 2
    };
    ASSERT_EQ(rect.position, expected_position);
    window->Destroy();
}


TEST_F(WindowTest, DefaultRect_Destroying_NoOwner) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();

    zaf::Rect rect;
    auto sub = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        rect = window->Rect();
    });
    window->Destroy();

    ASSERT_EQ(rect.size, DefaultWindowSize);
    auto dpi = window->GetDPI();
    float screen_width = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CXSCREEN)), dpi);
    float screen_height = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CYSCREEN)), dpi);
    zaf::Point expected_position{
        (screen_width - DefaultWindowSize.width) / 2,
        (screen_height - DefaultWindowSize.height) / 2
    };
    ASSERT_EQ(rect.position, expected_position);
}


TEST_F(WindowTest, DefaultRect_Destroying_Owner) {

    auto owner = zaf::Create<zaf::Window>();
    owner->SetRect(zaf::Rect{ 100, 100, 800, 600 });
    auto owner_holder = owner->CreateHandle();

    auto window = zaf::Create<zaf::Window>();
    window->SetOwner(owner);
    auto holder = window->CreateHandle();

    zaf::Rect rect;
    auto sub = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        rect = window->Rect();
    });

    window->Destroy();
    ASSERT_EQ(rect.size, DefaultWindowSize);
    zaf::Point expected_position{
        100 + (800 - DefaultWindowSize.width) / 2,
        100 + (600 - DefaultWindowSize.height) / 2
    };
    ASSERT_EQ(rect.position, expected_position);
}


TEST_F(WindowTest, DefaultRect_Destroyed) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->Destroy();
    ASSERT_EQ(window->Rect(), zaf::Rect{});
}


TEST_F(WindowTest, GetRectAfterChanging) {

    //Simulate user resize the window.
    {
        auto window = zaf::Create<zaf::Window>();
        window->SetRect(zaf::Rect{ 100, 100, 400, 400 });

        auto holder = window->CreateHandle();
        SetWindowPos(
            window->Handle(),
            nullptr,
            0,
            0,
            static_cast<int>(zaf::FromDIPs(400, window->GetDPI())),
            static_cast<int>(zaf::FromDIPs(500, window->GetDPI())),
            SWP_NOMOVE | SWP_NOACTIVATE);

        zaf::Rect new_rect{ 100, 100, 400, 500 };
        ASSERT_EQ(window->Rect(), new_rect);

        window->Destroy();
    }

    //Simulate user move the window.
    {
        auto window = zaf::Create<zaf::Window>();
        window->SetRect(zaf::Rect{ 100, 100, 400, 400 });

        auto holder = window->CreateHandle();
        SetWindowPos(
            window->Handle(),
            nullptr,
            static_cast<int>(zaf::FromDIPs(200, window->GetDPI())),
            static_cast<int>(zaf::FromDIPs(300, window->GetDPI())),
            0,
            0,
            SWP_NOSIZE | SWP_NOACTIVATE);

        zaf::Rect new_rect{ 200, 300, 400, 400 };
        ASSERT_EQ(window->Rect(), new_rect);

        window->Destroy();
    }
}


TEST_F(WindowTest, SetRect_NotCreated) {

    auto window = zaf::Create<zaf::Window>();
    zaf::Rect new_rect{ 100, 200, 300, 400 };
    window->SetRect(new_rect);
    ASSERT_EQ(window->Rect(), new_rect);

    // Creating state
    zaf::Rect rect_in_creating;
    auto sub = window->HandleCreatingEvent().Subscribe([&](const zaf::HandleCreatingInfo&) {
        rect_in_creating = window->Rect();
    });
    auto holder = window->CreateHandle();
    //ASSERT_EQ(rect_in_creating, new_rect);

    // Created state
    ASSERT_EQ(window->Rect(), new_rect);

    // Destroying state
    zaf::Rect rect_in_destroying;
    auto sub2 = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        rect_in_destroying = window->Rect();
    });
    window->Destroy();
    ASSERT_EQ(rect_in_destroying, new_rect);

    // Destroyed state
    ASSERT_EQ(window->Rect(), zaf::Rect{});
}


TEST_F(WindowTest, SetRect_Created) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    zaf::Rect new_rect{ 100, 200, 300, 400 };
    window->SetRect(new_rect);
    ASSERT_EQ(window->Rect(), new_rect);
    window->Destroy();
}


TEST_F(WindowTest, SetRect_Destroying) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    
    bool has_asserted{};
    auto sub = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        zaf::Rect new_rect{ 100, 200, 300, 400 };
        ASSERT_THROW(window->SetRect(new_rect), zaf::InvalidHandleStateError);
        has_asserted = true;
    });
    window->Destroy();
    ASSERT_TRUE(has_asserted);
}


TEST_F(WindowTest, SetRect_Destroyed) {
    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->Destroy();
    zaf::Rect new_rect{ 100, 200, 300, 400 };
    ASSERT_THROW(window->SetRect(new_rect), zaf::InvalidHandleStateError);
}


TEST_F(WindowTest, SetRect_Snaping_BeforeCreate) {

    auto window = zaf::Create<zaf::Window>();

    zaf::Rect initial_rect{ 10.25, 20.25, 400.25, 500.25 };
    window->SetRect(initial_rect);
    ASSERT_EQ(window->Rect(), initial_rect);

    auto holder = window->CreateHandle();
    //After creating the window, Rect() should use the actual window rect.
    auto rect_after_create = zaf::SnapToPixels(initial_rect, window->GetDPI());
    ASSERT_EQ(window->Rect(), rect_after_create);
    window->Destroy();
}


TEST_F(WindowTest, SetRect_Snaping_AfterCreate) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();

    zaf::Rect rect{ 10.25, 20.25, 400.25, 500.25 };
    window->SetRect(rect);

    //After calling SetRect(), the rect should be adjusted to the actual window rect.
    auto rect_after_set = zaf::SnapToPixels(rect, window->GetDPI());
    ASSERT_EQ(window->Rect(), rect_after_set);

    window->Destroy();
}

}