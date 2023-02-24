#include <gtest/gtest.h>
#include <zaf/window/message_only_window.h>

TEST(MessageOnlyWindowTest, ReceiveMessage) {

    zaf::MessageOnlyWindow window;

    zaf::Message received_message;
    auto subscription = window.MessageReceivedEvent().Subscribe(
        [&received_message](const zaf::Message& message) {
    
        received_message = message;
    });

    SendMessage(window.Handle(), WM_USER + 123, 47, 39);

    ASSERT_EQ(received_message.WindowHandle(), window.Handle());
    ASSERT_EQ(received_message.ID(), WM_USER + 123);
    ASSERT_EQ(received_message.WParam(), 47);
    ASSERT_EQ(received_message.LParam(), 39);
}
