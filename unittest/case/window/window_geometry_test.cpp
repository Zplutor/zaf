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
    
    auto dpi = window->DPI();
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

    auto dpi = window->DPI();
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
    auto dpi = window->DPI();
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
            static_cast<int>(zaf::FromDIPs(400, window->DPI())),
            static_cast<int>(zaf::FromDIPs(500, window->DPI())),
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
            static_cast<int>(zaf::FromDIPs(200, window->DPI())),
            static_cast<int>(zaf::FromDIPs(300, window->DPI())),
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
    ASSERT_EQ(rect_in_creating, new_rect);

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

    // Before creating the window, the new rect should be snapped to pixels.
    zaf::Rect initial_rect{ 10.25, 20.25, 400.25, 500.25 };
    window->SetRect(initial_rect);
    auto snapped_rect = zaf::SnapToPixels(initial_rect, window->DPI());
    ASSERT_EQ(window->Rect(), snapped_rect);

    auto holder = window->CreateHandle();
    //After creating the window, Rect() should use the actual window rect.
    ASSERT_EQ(window->Rect(), snapped_rect);
    window->Destroy();
}


TEST_F(WindowTest, SetRect_Snaping_AfterCreate) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();

    zaf::Rect rect{ 10.25, 20.25, 400.25, 500.25 };
    window->SetRect(rect);

    //After calling SetRect(), the rect should be adjusted to the actual window rect.
    auto rect_after_set = zaf::SnapToPixels(rect, window->DPI());
    ASSERT_EQ(window->Rect(), rect_after_set);

    window->Destroy();
}


TEST_F(WindowTest, SetPosition_NotCreated) {

    auto window = zaf::Create<zaf::Window>();
    window->SetPosition(zaf::Point{ 10, 20 });
    ASSERT_EQ(
        window->Rect(), 
        (zaf::Rect{ 10, 20, DefaultWindowSize.width, DefaultWindowSize.height }));
}


TEST_F(WindowTest, SetPosition_Creating) {
    //TODO
}


TEST_F(WindowTest, SetPosition_Created) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->SetPosition(zaf::Point{ 10, 20 });
    ASSERT_EQ(
        window->Rect(),
        (zaf::Rect{ 10, 20, DefaultWindowSize.width, DefaultWindowSize.height }));

    window->Destroy();
}


TEST_F(WindowTest, SetPosition_Destroying) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    bool has_asserted{};
    auto sub = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        ASSERT_THROW(window->SetPosition(zaf::Point{ 10, 20 }), zaf::InvalidHandleStateError);
        has_asserted = true;
    });
    window->Destroy();
    ASSERT_TRUE(has_asserted);
}


TEST_F(WindowTest, SetPosition_Destroyed) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->Destroy();
    ASSERT_THROW(window->SetPosition(zaf::Point{ 10, 20 }), zaf::InvalidHandleStateError);
}


TEST_F(WindowTest, SetSize_NotCreated) {

    auto window = zaf::Create<zaf::Window>();
    zaf::Size new_size{ 300, 400 };
    window->SetSize(new_size);

    auto dpi = window->DPI();
    float screen_width = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CXSCREEN)), dpi);
    float screen_height = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CYSCREEN)), dpi);
    zaf::Point position{
        (screen_width - new_size.width) / 2,
        (screen_height - new_size.height) / 2
    };
    ASSERT_EQ(window->Rect(), (zaf::Rect{ position, new_size }));
}


TEST_F(WindowTest, SetSize_Creating) {
    //TODO
}


TEST_F(WindowTest, SetSize_Created) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    zaf::Size new_size{ 300, 400 };
    window->SetSize(new_size);

    auto dpi = window->DPI();
    float screen_width = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CXSCREEN)), dpi);
    float screen_height = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CYSCREEN)), dpi);
    // Position should not changed after setting size, so we calculate the position based on the 
    // default size.
    zaf::Point position{
        (screen_width - DefaultWindowSize.width) / 2,
        (screen_height - DefaultWindowSize.height) / 2
    };
    ASSERT_EQ(window->Rect(), (zaf::Rect{ position, new_size }));
    window->Destroy();
}


TEST_F(WindowTest, SetSize_Destroying) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    bool has_asserted{};
    auto sub = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        ASSERT_THROW(window->SetSize(zaf::Size{ 300, 400 }), zaf::InvalidHandleStateError);
        has_asserted = true;
    });
    window->Destroy();
    ASSERT_TRUE(has_asserted);
}


TEST_F(WindowTest, SetSize_Destroyed) {
    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->Destroy();
    ASSERT_THROW(window->SetSize(zaf::Size{ 300, 400 }), zaf::InvalidHandleStateError);
}


TEST_F(WindowTest, SetWidth_NotCreated) {
    auto window = zaf::Create<zaf::Window>();
    window->SetWidth(300);
    ASSERT_EQ(window->Size(), (zaf::Size{ 300, DefaultWindowSize.height }));
}


TEST_F(WindowTest, SetWidth_Creating) {
    //TODO
}


TEST_F(WindowTest, SetWidth_Created) {
    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->SetWidth(300);
    ASSERT_EQ(window->Size(), (zaf::Size{ 300, DefaultWindowSize.height }));
    window->Destroy();
}


TEST_F(WindowTest, SetWidth_Destroying) {
    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    bool has_asserted{};
    auto sub = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        ASSERT_THROW(window->SetWidth(300), zaf::InvalidHandleStateError);
        has_asserted = true;
    });
    window->Destroy();
    ASSERT_TRUE(has_asserted);
}


TEST_F(WindowTest, SetWidth_Destroyed) {
    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->Destroy();
    ASSERT_THROW(window->SetWidth(300), zaf::InvalidHandleStateError);
}


TEST_F(WindowTest, SetHeight_NotCreated) {
    auto window = zaf::Create<zaf::Window>();
    window->SetHeight(400);
    ASSERT_EQ(window->Size(), (zaf::Size{ DefaultWindowSize.width, 400 }));
}


TEST_F(WindowTest, SetHeight_Creating) {
    //TODO
}


TEST_F(WindowTest, SetHeight_Created) {
    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->SetHeight(400);
    ASSERT_EQ(window->Size(), (zaf::Size{ DefaultWindowSize.width, 400 }));
    window->Destroy();
}


TEST_F(WindowTest, SetHeight_Destroying) {
    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    bool has_asserted{};
    auto sub = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        ASSERT_THROW(window->SetHeight(400), zaf::InvalidHandleStateError);
        has_asserted = true;
        });
    window->Destroy();
    ASSERT_TRUE(has_asserted);
}


TEST_F(WindowTest, SetHeight_Destroyed) {
    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->Destroy();
    ASSERT_THROW(window->SetHeight(400), zaf::InvalidHandleStateError);
}


TEST_F(WindowTest, MinWidth_DefaultValue) {
    auto window = zaf::Create<zaf::Window>();
    auto dpi = static_cast<float>(GetDpiForSystem());
    auto default_value = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CXMINTRACK)), dpi);
    ASSERT_EQ(window->MinWidth(), default_value);
}


TEST_F(WindowTest, SetMinWidth) {
    auto window = zaf::Create<zaf::Window>();
    window->SetMaxWidth(400);
    window->SetWidth(300);
    window->SetMinWidth(500);
    ASSERT_EQ(window->MinWidth(), 500);
    ASSERT_EQ(window->MaxWidth(), 500);
    ASSERT_EQ(window->Width(), 500);
}


TEST_F(WindowTest, SetMinWidth_InvalidStates) {

    auto window = zaf::Create<zaf::Window>();
    //Destroying state
    auto holder = window->CreateHandle();
    bool has_asserted{};
    auto sub = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        ASSERT_THROW(window->SetMinWidth(200), zaf::InvalidHandleStateError);
        has_asserted = true;
    });
    window->Destroy();
    ASSERT_TRUE(has_asserted);
    //Destroyed state
    ASSERT_THROW(window->SetMinWidth(200), zaf::InvalidHandleStateError);
}


TEST_F(WindowTest, MaxWidth_DefaultValue) {
    auto window = zaf::Create<zaf::Window>();
    auto dpi = static_cast<float>(GetDpiForSystem());
    auto default_value = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CXMAXTRACK)), dpi);
    ASSERT_EQ(window->MaxWidth(), default_value);
}


TEST_F(WindowTest, SetMaxWidth) {
    auto window = zaf::Create<zaf::Window>();
    window->SetMinWidth(400);
    window->SetWidth(500);
    window->SetMaxWidth(300);
    ASSERT_EQ(window->MaxWidth(), 300);
    ASSERT_EQ(window->MinWidth(), 300);
}


TEST_F(WindowTest, SetMaxWidth_InvalidStates) {

    auto window = zaf::Create<zaf::Window>();
    //Destroying state
    auto holder = window->CreateHandle();
    bool has_asserted{};
    auto sub = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        ASSERT_THROW(window->SetMaxWidth(800), zaf::InvalidHandleStateError);
        has_asserted = true;
    });
    window->Destroy();
    ASSERT_TRUE(has_asserted);
    //Destroyed state
    ASSERT_THROW(window->SetMaxWidth(800), zaf::InvalidHandleStateError);
}


TEST_F(WindowTest, MinHeight_DefaultValue) {
    auto window = zaf::Create<zaf::Window>();
    auto dpi = static_cast<float>(GetDpiForSystem());
    auto default_value = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CYMINTRACK)), dpi);
    ASSERT_EQ(window->MinHeight(), default_value);
}


TEST_F(WindowTest, SetMinHeight) {
    auto window = zaf::Create<zaf::Window>();
    window->SetMaxHeight(400);
    window->SetHeight(300);
    window->SetMinHeight(500);
    ASSERT_EQ(window->MinHeight(), 500);
    ASSERT_EQ(window->MaxHeight(), 500);
    ASSERT_EQ(window->Height(), 500);
}


TEST_F(WindowTest, SetMinHeight_InvalidStates) {

    auto window = zaf::Create<zaf::Window>();
    //Destroying state
    auto holder = window->CreateHandle();
    bool has_asserted{};
    auto sub = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        ASSERT_THROW(window->SetMinHeight(200), zaf::InvalidHandleStateError);
        has_asserted = true;
    });
    window->Destroy();
    ASSERT_TRUE(has_asserted);
    //Destroyed state
    ASSERT_THROW(window->SetMinHeight(200), zaf::InvalidHandleStateError);
}


TEST_F(WindowTest, MaxHeight_DefaultValue) {
    auto window = zaf::Create<zaf::Window>();
    auto dpi = static_cast<float>(GetDpiForSystem());
    auto default_value = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CYMAXTRACK)), dpi);
    ASSERT_EQ(window->MaxHeight(), default_value);
}


TEST_F(WindowTest, SetMaxHeight) {
    auto window = zaf::Create<zaf::Window>();
    window->SetMinHeight(400);
    window->SetHeight(500);
    window->SetMaxHeight(300);
    ASSERT_EQ(window->MaxHeight(), 300);
    ASSERT_EQ(window->MinHeight(), 300);
    ASSERT_EQ(window->Height(), 300);
}


TEST_F(WindowTest, SetMaxHeight_InvalidStates) {

    auto window = zaf::Create<zaf::Window>();
    //Destroying state
    auto holder = window->CreateHandle();
    bool has_asserted{};
    auto sub = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        ASSERT_THROW(window->SetMaxHeight(800), zaf::InvalidHandleStateError);
        has_asserted = true;
    });
    window->Destroy();
    ASSERT_TRUE(has_asserted);
    //Destroyed state
    ASSERT_THROW(window->SetMaxHeight(800), zaf::InvalidHandleStateError);
}


TEST_F(WindowTest, MinSize_DefaultValue) {
    auto window = zaf::Create<zaf::Window>();
    auto dpi = static_cast<float>(GetDpiForSystem());
    auto default_width = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CXMINTRACK)), dpi);
    auto default_height = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CYMINTRACK)), dpi);
    ASSERT_EQ(window->MinSize(), (zaf::Size{ default_width, default_height }));
}


TEST_F(WindowTest, SetMinSize) {
    auto window = zaf::Create<zaf::Window>();
    window->SetMaxSize(zaf::Size{ 400, 500 });
    window->SetSize(zaf::Size{ 300, 400 });
    window->SetMinSize(zaf::Size{ 500, 600 });
    ASSERT_EQ(window->MinSize(), (zaf::Size{ 500, 600 }));
    ASSERT_EQ(window->MaxSize(), (zaf::Size{ 500, 600 }));
    ASSERT_EQ(window->Size(), (zaf::Size{ 500, 600 }));
}


TEST_F(WindowTest, SetMinSize_InvalidStates) {
    auto window = zaf::Create<zaf::Window>();
    //Destroying state
    auto holder = window->CreateHandle();
    bool has_asserted{};
    auto sub = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        ASSERT_THROW(window->SetMinSize(zaf::Size{ 200, 300 }), zaf::InvalidHandleStateError);
        has_asserted = true;
    });
    window->Destroy();
    ASSERT_TRUE(has_asserted);
    //Destroyed state
    ASSERT_THROW(window->SetMinSize(zaf::Size{ 200, 300 }), zaf::InvalidHandleStateError);
}


TEST_F(WindowTest, MaxSize_DefaultValue) {
    auto window = zaf::Create<zaf::Window>();
    auto dpi = static_cast<float>(GetDpiForSystem());
    auto default_width = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CXMAXTRACK)), dpi);
    auto default_height = zaf::ToDIPs(static_cast<float>(GetSystemMetrics(SM_CYMAXTRACK)), dpi);
    ASSERT_EQ(window->MaxSize(), (zaf::Size{ default_width, default_height }));
}


TEST_F(WindowTest, SetMaxSize) {
    auto window = zaf::Create<zaf::Window>();
    window->SetMinSize(zaf::Size{ 400, 500 });
    window->SetSize(zaf::Size{ 500, 600 });
    window->SetMaxSize(zaf::Size{ 300, 400 });
    ASSERT_EQ(window->MaxSize(), (zaf::Size{ 300, 400 }));
    ASSERT_EQ(window->MinSize(), (zaf::Size{ 300, 400 }));
    ASSERT_EQ(window->Size(), (zaf::Size{ 300, 400 }));
}


TEST_F(WindowTest, SetMaxSize_InvalidStates) {
    auto window = zaf::Create<zaf::Window>();
    //Destroying state
    auto holder = window->CreateHandle();
    bool has_asserted{};
    auto sub = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        ASSERT_THROW(window->SetMaxSize(zaf::Size{ 800, 900 }), zaf::InvalidHandleStateError);
        has_asserted = true;
    });
    window->Destroy();
    ASSERT_TRUE(has_asserted);
    //Destroyed state
    ASSERT_THROW(window->SetMaxSize(zaf::Size{ 800, 900 }), zaf::InvalidHandleStateError);
}


TEST_F(WindowTest, SetRect_MinSize) {

    auto window = zaf::Create<zaf::Window>();
    window->SetMinSize({ 300, 300 });

    // Not created
    window->SetRect({ 0, 0, 200, 200 });
    ASSERT_EQ(window->Size(), (zaf::Size{ 300, 300 }));

    // Created
    auto holder = window->CreateHandle();
    window->SetRect({ 0, 0, 200, 200 });
    ASSERT_EQ(window->Size(), (zaf::Size{ 300, 300 }));
}


TEST_F(WindowTest, SetRect_MaxSize) {

    auto window = zaf::Create<zaf::Window>();
    window->SetMaxSize({ 400, 400 });

    // Not created
    window->SetRect({ 0, 0, 500, 500 });
    ASSERT_EQ(window->Size(), (zaf::Size{ 400, 400 }));

    // Created
    auto holder = window->CreateHandle();
    window->SetRect({ 0, 0, 500, 500 });
    ASSERT_EQ(window->Size(), (zaf::Size{ 400, 400 }));
}


TEST_F(WindowTest, SetSize_MinSize) {

    auto window = zaf::Create<zaf::Window>();
    window->SetMinSize({ 300, 300 });

    // Not created
    window->SetSize({ 200, 200 });
    ASSERT_EQ(window->Size(), (zaf::Size{ 300, 300 }));

    // Created
    auto holder = window->CreateHandle();
    window->SetSize({ 200, 200 });
    ASSERT_EQ(window->Size(), (zaf::Size{ 300, 300 }));
}


TEST_F(WindowTest, SetSize_MaxSize) {

    auto window = zaf::Create<zaf::Window>();
    window->SetMaxSize({ 400, 400 });

    // Not created
    window->SetSize({ 500, 500 });
    ASSERT_EQ(window->Size(), (zaf::Size{ 400, 400 }));

    // Created
    auto holder = window->CreateHandle();
    window->SetSize({ 500, 500 });
    ASSERT_EQ(window->Size(), (zaf::Size{ 400, 400 }));
}


TEST_F(WindowTest, SetWidth_MinWidth) {

    auto window = zaf::Create<zaf::Window>();
    window->SetMinWidth(500);

    // Not created
    window->SetWidth(400);
    ASSERT_EQ(window->Width(), 500);

    // Created
    auto holder = window->CreateHandle();
    window->SetWidth(400);
    ASSERT_EQ(window->Width(), 500);
}


TEST_F(WindowTest, SetWidth_MaxWidth) {

    auto window = zaf::Create<zaf::Window>();
    window->SetMaxWidth(500);

    // Not created
    window->SetWidth(600);
    ASSERT_EQ(window->Width(), 500);

    // Created
    auto holder = window->CreateHandle();
    window->SetWidth(600);
    ASSERT_EQ(window->Width(), 500);
}


TEST_F(WindowTest, SetHeight_MinHeight) {

    auto window = zaf::Create<zaf::Window>();
    window->SetMinHeight(500);

    // Not created
    window->SetHeight(400);
    ASSERT_EQ(window->Height(), 500);

    // Created
    auto holder = window->CreateHandle();
    window->SetHeight(400);
    ASSERT_EQ(window->Height(), 500);
}


TEST_F(WindowTest, SetHeight_MaxHeight) {

    auto window = zaf::Create<zaf::Window>();
    window->SetMaxHeight(500);

    // Not created
    window->SetHeight(600);
    ASSERT_EQ(window->Height(), 500);

    // Created
    auto holder = window->CreateHandle();
    window->SetHeight(600);
    ASSERT_EQ(window->Height(), 500);
}


TEST_F(WindowTest, ContentRect) {

    auto window = zaf::Create<zaf::Window>();
    auto window_size = window->Size();

    // Not Created
    auto content_rect_not_created = window->ContentRect();
    ASSERT_EQ(content_rect_not_created.position, (zaf::Point{ 0, 0 }));
    ASSERT_LT(content_rect_not_created.size.width, window_size.Width());
    ASSERT_LT(content_rect_not_created.size.height, window_size.Height());

    // Creating
    zaf::Rect content_rect_in_creating;
    auto sub = window->HandleCreatingEvent().Subscribe([&](const zaf::HandleCreatingInfo&) {
        content_rect_in_creating = window->ContentRect();
    });
    auto holder = window->CreateHandle();
    ASSERT_EQ(content_rect_in_creating.position, (zaf::Point{ 0, 0 }));
    ASSERT_LT(content_rect_in_creating.size.width, window_size.Width());
    ASSERT_LT(content_rect_in_creating.size.height, window_size.Height());

    // Created
    auto content_rect_created = window->ContentRect();
    ASSERT_EQ(content_rect_created.position, (zaf::Point{ 0, 0 }));
    ASSERT_LT(content_rect_created.size.width, window_size.Width());
    ASSERT_LT(content_rect_created.size.height, window_size.Height());

    // Destroying
    zaf::Rect content_rect_in_destroying;
    auto sub2 = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        content_rect_in_destroying = window->ContentRect();
    });
    window->Destroy();
    ASSERT_EQ(content_rect_in_destroying.position, (zaf::Point{ 0, 0 }));
    ASSERT_LT(content_rect_in_destroying.size.width, window_size.Width());
    ASSERT_LT(content_rect_in_destroying.size.width, window_size.Width());

    // Destroyed
    ASSERT_EQ(window->ContentRect(), zaf::Rect{});

    //ASSERT_EQ(content_rect_not_created, content_rect_in_creating);
    ASSERT_EQ(content_rect_not_created, content_rect_created);
    ASSERT_EQ(content_rect_not_created, content_rect_in_destroying);
}


TEST_F(WindowTest, ContentSize) {

    auto window = zaf::Create<zaf::Window>();
    auto window_size = window->Size();

    // Not Created
    auto content_size_not_created = window->ContentSize();
    ASSERT_LT(content_size_not_created.width, window_size.Width());
    ASSERT_LT(content_size_not_created.height, window_size.Height());

    // Creating
    zaf::Size content_size_in_creating;
    auto sub = window->HandleCreatingEvent().Subscribe([&](const zaf::HandleCreatingInfo&) {
        content_size_in_creating = window->ContentSize();
    });
    auto holder = window->CreateHandle();
    ASSERT_LT(content_size_in_creating.width, window_size.Width());
    ASSERT_LT(content_size_in_creating.height, window_size.Height());

    // Created
    auto content_size_created = window->ContentSize();
    ASSERT_LT(content_size_created.width, window_size.Width());
    ASSERT_LT(content_size_created.height, window_size.Height());

    // Destroying
    zaf::Size content_size_in_destroying;
    auto sub2 = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        content_size_in_destroying = window->ContentSize();
    });
    window->Destroy();
    ASSERT_LT(content_size_in_destroying.width, window_size.Width());
    ASSERT_LT(content_size_in_destroying.height, window_size.Height());

    // Destroyed
    ASSERT_EQ(window->ContentSize(), zaf::Size{});

    //ASSERT_EQ(content_size_not_created, content_size_in_creating);
    ASSERT_EQ(content_size_not_created, content_size_created);
    ASSERT_EQ(content_size_not_created, content_size_in_destroying);
}


TEST_F(WindowTest, SetContentSize_NotCreate) {

    auto window = zaf::Create<zaf::Window>();

    zaf::Size content_size{ 300, 400 };
    window->SetContentSize(content_size);
    ASSERT_EQ(window->ContentSize(), content_size);

    auto window_size = window->Size();
    ASSERT_GT(window_size.width, content_size.width);
    ASSERT_GT(window_size.height, content_size.height);

    // After creating the handle, the content size should remain unchanged.
    auto holder = window->CreateHandle();
    ASSERT_EQ(window->ContentSize(), content_size);
    window_size = window->Size();
    ASSERT_GT(window_size.width, content_size.width);
    ASSERT_GT(window_size.height, content_size.height);
    window->Destroy();
}


TEST_F(WindowTest, SetContentSize_Created) {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    auto position = window->Position();

    zaf::Size content_size{ 300, 400 };
    window->SetContentSize(content_size);
    ASSERT_EQ(window->ContentSize(), content_size);
    // Position should not change after setting content size.
    ASSERT_EQ(window->Position(), position);

    auto window_size = window->Size();
    ASSERT_GT(window_size.width, content_size.width);
    ASSERT_GT(window_size.height, content_size.height);
    window->Destroy();
}


TEST_F(WindowTest, SetContentSize_InvalidStates) {

    auto window = zaf::Create<zaf::Window>();

    //Destroying state
    auto holder = window->CreateHandle();
    bool has_asserted{};
    auto sub = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        ASSERT_THROW(window->SetContentSize(zaf::Size{ 300, 400 }), zaf::InvalidHandleStateError);
        has_asserted = true;
    });
    window->Destroy();
    ASSERT_TRUE(has_asserted);

    //Destroyed state
    ASSERT_THROW(window->SetContentSize(zaf::Size{ 300, 400 }), zaf::InvalidHandleStateError);
}


TEST_F(WindowTest, SizeChangedEvent_SetRect_NotCreated) {

    auto window = zaf::Create<zaf::Window>();
    int call_count{};
    auto sub = window->SizeChangedEvent().Subscribe([&](const zaf::WindowSizeChangedInfo&) {
        call_count++;
    });

    window->SetRect(zaf::Rect{ 0, 0, 100, 100 });
    ASSERT_EQ(call_count, 1);
    window->SetRect(zaf::Rect{ 0, 0, 200, 100 });
    ASSERT_EQ(call_count, 2);
    window->SetRect(zaf::Rect{ 0, 0, 200, 200 });
    ASSERT_EQ(call_count, 3);
    // Setting position or setting the same size won't raise the event.
    window->SetRect(zaf::Rect{ 100, 100, 200, 200 });
    ASSERT_EQ(call_count, 3);
}


TEST_F(WindowTest, SizeChangedEvent_SetRect_Creating) {

    auto window = zaf::Create<zaf::Window>();
    int call_count{};
    auto sub1 = window->SizeChangedEvent().Subscribe([&](const zaf::WindowSizeChangedInfo&) {
        call_count++;
    });

    auto sub2 = window->HandleCreatingEvent().Subscribe([&](const zaf::HandleCreatingInfo&) {
        window->SetRect(zaf::Rect{ 0, 0, 100, 100 });
        ASSERT_EQ(call_count, 1);
        window->SetRect(zaf::Rect{ 0, 0, 200, 100 });
        ASSERT_EQ(call_count, 2);
        window->SetRect(zaf::Rect{ 0, 0, 200, 200 });
        ASSERT_EQ(call_count, 3);
        // Setting position or setting the same size won't raise the event.
        window->SetRect(zaf::Rect{ 100, 100, 200, 200 });
        ASSERT_EQ(call_count, 3);
    });

    auto holder = window->CreateHandle();
    ASSERT_EQ(call_count, 3);
}


TEST_F(WindowTest, SizeChangedEvent_SetRect_Created) {

    auto window = zaf::Create<zaf::Window>();
    int call_count{};
    auto sub = window->SizeChangedEvent().Subscribe([&](const zaf::WindowSizeChangedInfo&) {
        call_count++;
    });
    auto holder = window->CreateHandle();
    window->SetRect(zaf::Rect{ 0, 0, 100, 100 });
    ASSERT_EQ(call_count, 1);
    window->SetRect(zaf::Rect{ 0, 0, 200, 100 });
    ASSERT_EQ(call_count, 2);
    window->SetRect(zaf::Rect{ 0, 0, 200, 200 });
    ASSERT_EQ(call_count, 3);
    // Setting position or setting the same size won't raise the event.
    window->SetRect(zaf::Rect{ 100, 100, 200, 200 });
    ASSERT_EQ(call_count, 3);
}


TEST_F(WindowTest, SizeChangedEvent_SetSize_NotCreated) {

    auto window = zaf::Create<zaf::Window>();
    int call_count{};
    auto sub = window->SizeChangedEvent().Subscribe([&](const zaf::WindowSizeChangedInfo&) {
        call_count++;
    });

    window->SetSize(zaf::Size{ 100, 100 });
    ASSERT_EQ(call_count, 1);
    window->SetSize(zaf::Size{ 200, 100 });
    ASSERT_EQ(call_count, 2);
    window->SetSize(zaf::Size{ 200, 200 });
    ASSERT_EQ(call_count, 3);
    // Setting the same size won't raise the event.
    window->SetSize(zaf::Size{ 200, 200 });
    ASSERT_EQ(call_count, 3);
}


TEST_F(WindowTest, SizeChangedEvent_SetSize_Creating) {

    auto window = zaf::Create<zaf::Window>();
    int call_count{};
    auto sub1 = window->SizeChangedEvent().Subscribe([&](const zaf::WindowSizeChangedInfo&) {
        call_count++;
    });

    auto sub2 = window->HandleCreatingEvent().Subscribe([&](const zaf::HandleCreatingInfo&) {
        window->SetSize(zaf::Size{ 100, 100 });
        ASSERT_EQ(call_count, 1);
        window->SetSize(zaf::Size{ 200, 100 });
        ASSERT_EQ(call_count, 2);
        window->SetSize(zaf::Size{ 200, 200 });
        ASSERT_EQ(call_count, 3);
        // Setting the same size won't raise the event.
        window->SetSize(zaf::Size{ 200, 200 });
        ASSERT_EQ(call_count, 3);
    });
    auto holder = window->CreateHandle();
    ASSERT_EQ(call_count, 3);
}


TEST_F(WindowTest, SizeChangedEvent_SetSize_Created) {

    auto window = zaf::Create<zaf::Window>();
    int call_count{};
    auto sub = window->SizeChangedEvent().Subscribe([&](const zaf::WindowSizeChangedInfo&) {
        call_count++;
    });

    auto holder = window->CreateHandle();
    window->SetSize(zaf::Size{ 100, 100 });
    ASSERT_EQ(call_count, 1);
    window->SetSize(zaf::Size{ 200, 100 });
    ASSERT_EQ(call_count, 2);
    window->SetSize(zaf::Size{ 200, 200 });
    ASSERT_EQ(call_count, 3);
    // Setting the same size won't raise the event.
    window->SetSize(zaf::Size{ 200, 200 });
    ASSERT_EQ(call_count, 3);
}


TEST_F(WindowTest, WhenNotSizingOrMoving_InVariantStates) {

    auto window = zaf::Create<zaf::Window>();
    zaf::rx::DisposeBag dispose_bag;

    // NotCreated
    bool not_created_called{};
    dispose_bag += window->WhenNotSizingOrMoving().Subscribe([&](zaf::None) {
        not_created_called = true;
    });
    ASSERT_TRUE(not_created_called);

    // Creating
    bool creating_called{};
    dispose_bag += window->HandleCreatingEvent().Subscribe([&](const zaf::HandleCreatingInfo&) {
        dispose_bag += window->WhenNotSizingOrMoving().Subscribe([&](zaf::None) {
            creating_called = true;
        });
    });
    auto holder = window->CreateHandle();
    ASSERT_TRUE(creating_called);

    // Created
    bool created_called{};
    dispose_bag += window->WhenNotSizingOrMoving().Subscribe([&](zaf::None) {
        created_called = true;
    });
    ASSERT_TRUE(created_called);

    // Destroying
    bool destroying_called{};
    dispose_bag += window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
        dispose_bag += window->WhenNotSizingOrMoving().Subscribe([&](zaf::None) {
            destroying_called = true;
        });
    });
    window->Destroy();
    ASSERT_TRUE(destroying_called);

    // Destroyed
    bool destroyed_called{};
    dispose_bag += window->WhenNotSizingOrMoving().Subscribe([&](zaf::None) {
        destroyed_called = true;
    });
    ASSERT_TRUE(destroyed_called);
}


TEST_F(WindowTest, WhenNotSizingOrMoving_BeingSizedOrMoved) {

    //Event doesn't emit if the window is being resized or moved.
    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->Messager().Send(WM_ENTERSIZEMOVE, 0, 0);

    int call_count{};
    auto sub = window->WhenNotSizingOrMoving().Subscribe([&](zaf::None) {
        call_count++;
    });
    ASSERT_EQ(call_count, 0);

    //Event emits when the window finishes resizing or moving.
    window->Messager().Send(WM_EXITSIZEMOVE, 0, 0);
    ASSERT_EQ(call_count, 1);

    //Previous observable should not emit again.
    window->Messager().Send(WM_ENTERSIZEMOVE, 0, 0);
    window->Messager().Send(WM_EXITSIZEMOVE, 0, 0);
    ASSERT_EQ(call_count, 1);
}


TEST_F(WindowTest, WhenNotSizingOrMoving_DestroyWhileBeingSizedOrMoved) {

    //Event doesn't emit if the window is destroy during resizing or moving.
    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();
    window->Messager().Send(WM_ENTERSIZEMOVE, 0, 0);

    int call_count{};
    auto subscription = window->WhenNotSizingOrMoving().Subscribe([&](zaf::None) {
        call_count++;
    });
    window->Destroy();
    ASSERT_EQ(call_count, 0);
}


namespace {

zaf::Point GetWindowContentOffset() {

    auto window = zaf::Create<zaf::Window>();
    auto holder = window->CreateHandle();

    RECT window_rect{};
    GetWindowRect(window->Handle(), &window_rect);

    RECT client_rect{};
    GetClientRect(window->Handle(), &client_rect);

    POINT client_origin{ client_rect.left, client_rect.top };
    ClientToScreen(window->Handle(), &client_origin);

    zaf::Point result{ 
        static_cast<float>(client_origin.x - window_rect.left),
        static_cast<float>(client_origin.y - window_rect.top) 
    };
    return zaf::ToDIPs(result, window->DPI());
}

}


TEST_F(WindowTest, TransformPointFromScreen_InVariantStates) {

    auto test = [&](const zaf::Point& screen_point) {

        auto window = zaf::Create<zaf::Window>();
        window->SetPosition(zaf::Point{ 100, 100 });

        auto content_offset = GetWindowContentOffset();
        zaf::Point expected{
            screen_point.x - 100.f - content_offset.x,
            screen_point.y - 100.f - content_offset.y
        };

        // Not created
        auto point_in_not_created = window->TransformFromScreen(screen_point);
        ASSERT_EQ(point_in_not_created, expected);

        // Creating
        zaf::Point point_in_creating;
        auto sub1 = window->HandleCreatingEvent().Subscribe([&](const zaf::HandleCreatingInfo&) {
            point_in_creating = window->TransformFromScreen(screen_point);
        });
        auto holder = window->CreateHandle();
        ASSERT_EQ(point_in_creating, expected);

        // Created
        auto point_in_created = window->TransformFromScreen(screen_point);
        ASSERT_EQ(point_in_created, expected);

        // Destroying
        zaf::Point point_in_destroying;
        auto sub2 = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
            point_in_destroying = window->TransformFromScreen(screen_point);
        });
        window->Destroy();
        ASSERT_EQ(point_in_destroying, expected);

        // Destroyed
        auto point_in_destroyed = window->TransformFromScreen(screen_point);
        ASSERT_EQ(point_in_destroyed, screen_point);
    };

    test(zaf::Point{ 40.25f, 50.25f });
    test(zaf::Point{ 100.15f, 100.81f });
}


TEST_F(WindowTest, TransformPointToScreen_InVariantStates) {

    auto test = [&](const zaf::Point& window_point) {

        auto window = zaf::Create<zaf::Window>();
        window->SetPosition(zaf::Point{ 100, 100 });

        auto content_offset = GetWindowContentOffset();
        zaf::Point expected{
            window_point.x + 100.f + content_offset.x,
            window_point.y + 100.f + content_offset.y
        };

        // Not created
        auto point_in_not_created = window->TransformToScreen(window_point);
        ASSERT_EQ(point_in_not_created, expected);

        // Creating
        zaf::Point point_in_creating;
        auto sub1 = window->HandleCreatingEvent().Subscribe([&](const zaf::HandleCreatingInfo&) {
            point_in_creating = window->TransformToScreen(window_point);
        });
        auto holder = window->CreateHandle();
        ASSERT_EQ(point_in_creating, expected);

        // Created
        auto point_in_created = window->TransformToScreen(window_point);
        ASSERT_EQ(point_in_created, expected);

        // Destroying
        zaf::Point point_in_destroying;
        auto sub2 = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
            point_in_destroying = window->TransformToScreen(window_point);
        });
        window->Destroy();
        ASSERT_EQ(point_in_destroying, expected);

        // Destroyed
        auto point_in_destroyed = window->TransformToScreen(window_point);
        ASSERT_EQ(point_in_destroyed, window_point);
    };

    test(zaf::Point{ -10.25f, -11.35f });
    test(zaf::Point{ 10.15f, 11.45f });
}


TEST_F(WindowTest, TransformRectFromScreen_InVariantStates) {

    auto test = [&](const zaf::Rect& screen_rect) {

        auto window = zaf::Create<zaf::Window>();
        window->SetPosition(zaf::Point{ 100, 100 });

        auto content_offset = GetWindowContentOffset();
        zaf::Rect expected{
            screen_rect.position.x - 100.f - content_offset.x,
            screen_rect.position.y - 100.f - content_offset.y,
            screen_rect.size.width,
            screen_rect.size.height
        };

        // Not created
        auto rect_in_not_created = window->TransformFromScreen(screen_rect);
        ASSERT_EQ(rect_in_not_created, expected);

        // Creating
        zaf::Rect rect_in_creating;
        auto sub1 = window->HandleCreatingEvent().Subscribe([&](const zaf::HandleCreatingInfo&) {
            rect_in_creating = window->TransformFromScreen(screen_rect);
        });
        auto holder = window->CreateHandle();
        ASSERT_EQ(rect_in_creating, expected);

        // Created
        auto rect_in_created = window->TransformFromScreen(screen_rect);
        ASSERT_EQ(rect_in_created, expected);

        // Destroying
        zaf::Rect rect_in_destroying;
        auto sub2 = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
            rect_in_destroying = window->TransformFromScreen(screen_rect);
        });
        window->Destroy();
        ASSERT_EQ(rect_in_destroying, expected);

        // Destroyed
        auto rect_in_destroyed = window->TransformFromScreen(screen_rect);
        ASSERT_EQ(rect_in_destroyed, screen_rect);
    };
    test(zaf::Rect{ 40.25f, 50.25f, 100.f, 200.f });
    test(zaf::Rect{ 100.15f, 100.81f, 300.f, 400.f });
}


TEST_F(WindowTest, TransformRectToScreen_InVariantState) {

    auto test = [&](const zaf::Rect& window_rect) {

        auto window = zaf::Create<zaf::Window>();
        window->SetPosition(zaf::Point{ 100, 100 });

        auto content_offset = GetWindowContentOffset();
        zaf::Rect expected{
            window_rect.position.x + 100.f + content_offset.x,
            window_rect.position.y + 100.f + content_offset.y,
            window_rect.size.width,
            window_rect.size.height
        };

        // Not created
        auto rect_in_not_created = window->TransformToScreen(window_rect);
        ASSERT_EQ(rect_in_not_created, expected);

        // Creating
        zaf::Rect rect_in_creating;
        auto sub1 = window->HandleCreatingEvent().Subscribe([&](const zaf::HandleCreatingInfo&) {
            rect_in_creating = window->TransformToScreen(window_rect);
        });
        auto holder = window->CreateHandle();
        ASSERT_EQ(rect_in_creating, expected);

        // Created
        auto rect_in_created = window->TransformToScreen(window_rect);
        ASSERT_EQ(rect_in_created, expected);

        // Destroying
        zaf::Rect rect_in_destroying;
        auto sub2 = window->DestroyingEvent().Subscribe([&](const zaf::DestroyingInfo&) {
            rect_in_destroying = window->TransformToScreen(window_rect);
        });
        window->Destroy();
        ASSERT_EQ(rect_in_destroying, expected);

        // Destroyed
        auto rect_in_destroyed = window->TransformToScreen(window_rect);
        ASSERT_EQ(rect_in_destroyed, window_rect);
    };
    test(zaf::Rect{ -10.25f, -11.35f, 100.f, 200.f });
    test(zaf::Rect{ 10.15f, 11.45f, 300.f, 400.f });
}

}