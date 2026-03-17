#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/window/message/keyboard_message.h>

namespace zaf {
class Window;
}

namespace zaf::internal {

class WindowKeyboardFacet : NonCopyableNonMovable {
public:
    explicit WindowKeyboardFacet(Window& window) noexcept;

    bool PreprocessMessage(const KeyMessage& message);

    bool HandleKeyboardMessage(const Message& message);
    bool HandleIMEMessage(const Message& message);

private:
    bool TryToPreprocessInspectorShortcutMessage(const KeyMessage& message);

private:
    Window& window_;
};

}