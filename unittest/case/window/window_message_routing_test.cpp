#include <gtest/gtest.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include "window_test.h"

namespace zaf::testing {
namespace {

constexpr UINT TestMessage = WM_USER + 1;

class TestWindow : public zaf::Window {
public:
    std::size_t on_message_received1_sequence{};
    std::size_t on_message_received2_sequence{};
    std::size_t message_received_event_sequence{};

    std::size_t on_message_handled1_sequence{};
    std::size_t on_message_handled2_sequence{};
    std::size_t message_handled_event_sequence{};

protected:
    void Initialize() override {

        __super::Initialize();

        Disposables() += this->MessageReceivedEvent().Subscribe(
            [this](const zaf::MessageReceivedInfo& event_info) {

            if (event_info.Message().ID() == TestMessage) {
                message_received_event_sequence = ++current_sequence_;
            }
        });

        Disposables() += this->MessageHandledEvent().Subscribe(
            [this](const zaf::MessageHandledInfo& event_info) {

            if (event_info.Message().ID() == TestMessage) {
                message_handled_event_sequence = ++current_sequence_;
            }
        });
    }

    void OnMessageReceived(const zaf::MessageReceivedInfo& event_info) override {

        if (event_info.Message().ID() == TestMessage) {
            on_message_received1_sequence = ++current_sequence_;
        }

        __super::OnMessageReceived(event_info);

        if (event_info.Message().ID() == TestMessage) {
            on_message_received2_sequence = ++current_sequence_;
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

    ASSERT_EQ(window->on_message_received1_sequence, 1);
    ASSERT_EQ(window->message_received_event_sequence, 2);
    ASSERT_EQ(window->on_message_received2_sequence, 3);
    ASSERT_EQ(window->on_message_handled1_sequence, 4);
    ASSERT_EQ(window->message_handled_event_sequence, 5);
    ASSERT_EQ(window->on_message_handled2_sequence, 6);
}

}