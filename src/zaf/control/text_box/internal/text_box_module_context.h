#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf {

class TextBox;

namespace internal {

class TextBoxCore;
class TextBoxEditor;
class TextBoxMouseInputHandler;
class TextBoxKeyboardInputHandler;
class TextBoxSelectionManager;

class TextBoxModuleContext : NonCopyableNonMovable {
public:
    explicit TextBoxModuleContext(TextBox* owner, TextBoxCore* core);

    void Initialize();

    TextBox& Owner() const {
        return *owner_;
    }

    TextBoxCore& Core() const {
        return *core_;
    }

    TextBoxSelectionManager& SelectionManager() const {
        return *selection_manager_;
    }
    
    TextBoxMouseInputHandler& MouseInputHandler() const {
        return *mouse_input_handler_;
    }

    TextBoxKeyboardInputHandler& KeyboardInputHandler() const {
        return *keyboard_input_handler_;
    }

    TextBoxEditor& Editor() const {
        return *editor_;
    }

private:
    TextBox* owner_{};
    TextBoxCore* core_{};
    std::unique_ptr<TextBoxSelectionManager> selection_manager_;
    std::unique_ptr<TextBoxMouseInputHandler> mouse_input_handler_;
    std::unique_ptr<TextBoxKeyboardInputHandler> keyboard_input_handler_;
    std::unique_ptr<TextBoxEditor> editor_;
};

}
}