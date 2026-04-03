#include <gtest/gtest.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include "window_test.h"

namespace zaf::testing {
namespace {

constexpr UINT TestMessage = WM_USER + 1;

class TestWindow : public zaf::Window {
public:
    std::size_t on_message_handling1_sequence{};
    std::size_t on_message_handling2_sequence{};
    std::size_t message_handling_event_sequence{};

    std::size_t on_message_handled1_sequence{};
    std::size_t on_message_handled2_sequence{};
    std::size_t message_handled_event_sequence{};

protected:
    void Initialize() override {

        __super::Initialize();

        Disposables() += this->MessageHandlingEvent().Subscribe(
            [this](const zaf::MessageHandlingInfo& event_info) {

            if (event_info.Message().ID() == TestMessage) {
                message_handling_event_sequence = ++current_sequence_;
            }
        });

        Disposables() += this->MessageHandledEvent().Subscribe(
            [this](const zaf::MessageHandledInfo& event_info) {

            if (event_info.Message().ID() == TestMessage) {
                message_handled_event_sequence = ++current_sequence_;
            }
        });
    }

    void OnMessageHandling(const zaf::MessageHandlingInfo& event_info) override {

        if (event_info.Message().ID() == TestMessage) {
            on_message_handling1_sequence = ++current_sequence_;
        }

        __super::OnMessageHandling(event_info);

        if (event_info.Message().ID() == TestMessage) {
            on_message_handling2_sequence = ++current_sequence_;
        }
    }

    void OnMessageHandled(const zaf::MessageHandledInfo& event_info) override {

        if (event_info.Message().ID() == TestMessage) {
            on_message_handled1_sequence = ++current_sequence_;
        }

        __super::OnMessageHandled(event_info);

        if (event_info.Message().ID() == TestMessage) {
            on_message_handled2_sequence = ++current_sequence_;
        }
    }

private:
    std::size_t current_sequence_{};
};

} // namespace


TEST_F(WindowTest, MessageRoutingSequence) {

    auto window = zaf::Create<TestWindow>();
    auto holder = window->CreateHandle();

    SendMessage(window->Handle(), TestMessage, 0, 0);

    ASSERT_EQ(window->on_message_handling1_sequence, 1);
    ASSERT_EQ(window->message_handling_event_sequence, 2);
    ASSERT_EQ(window->on_message_handling2_sequence, 3);
    ASSERT_EQ(window->on_message_handled1_sequence, 4);
    ASSERT_EQ(window->message_handled_event_sequence, 5);
    ASSERT_EQ(window->on_message_handled2_sequence, 6);
}


TEST_F(WindowTest, MessageHandlingEvent_CannotBeHandledMessages) {

    auto window = zaf::Create<Window>();

    bool tested_wm_nccreate{};
    bool tested_wm_create{};
    bool tested_wm_destroy{};
    bool tested_wm_ncdestroy{};

    auto sub = window->MessageHandlingEvent().Subscribe(
        [&](const zaf::MessageHandlingInfo& event_info) {

        if (event_info.Message().ID() == WM_NCCREATE) {
            tested_wm_nccreate = true;
            ASSERT_FALSE(event_info.CanBeHandled());
            ASSERT_THROW(event_info.MarkAsHandled(0), zaf::InvalidOperationError);
        }
        else if (event_info.Message().ID() == WM_CREATE) {
            tested_wm_create = true;
            ASSERT_FALSE(event_info.CanBeHandled());
            ASSERT_THROW(event_info.MarkAsHandled(0), zaf::InvalidOperationError);
        }
        else if (event_info.Message().ID() == WM_DESTROY) {
            tested_wm_destroy = true;
            ASSERT_FALSE(event_info.CanBeHandled());
            ASSERT_THROW(event_info.MarkAsHandled(0), zaf::InvalidOperationError);
        }
        else if (event_info.Message().ID() == WM_NCDESTROY) {
            tested_wm_ncdestroy = true;
            ASSERT_FALSE(event_info.CanBeHandled());
            ASSERT_THROW(event_info.MarkAsHandled(0), zaf::InvalidOperationError);
        }
    });

    auto holder = window->CreateHandle();
    window->Destroy();

    ASSERT_TRUE(tested_wm_nccreate);
    ASSERT_TRUE(tested_wm_create);
    ASSERT_TRUE(tested_wm_destroy);
    ASSERT_TRUE(tested_wm_ncdestroy);
}

}