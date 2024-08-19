#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/text_box.h>
#include <zaf/graphic/text/text_layout.h>

namespace zaf {
namespace internal {

class TextBoxCaretManager;
class TextBoxEditor;
class TextBoxHitTestManager;
class TextBoxIMEManager;
class TextBoxIndexManager;
class TextBoxMouseInputHandler;
class TextBoxKeyboardInputHandler;
class TextBoxSelectionManager;

class TextBoxModuleContext : NonCopyableNonMovable {
public:
    explicit TextBoxModuleContext(TextBox* owner);
    ~TextBoxModuleContext();

    void Initialize();

    TextBox& Owner() const {
        return *owner_;
    }

    TextModel& TextModel() const {
        return owner_->TextModel();
    }

    TextLayout GetTextLayout() const {
        return owner_->GetTextLayout();
    }

    TextBoxHitTestManager& HitTestManager() const {
        return *hit_test_manager_;
    }

    TextBoxIndexManager& IndexManager() const {
        return *index_manager_;
    }

    TextBoxSelectionManager& SelectionManager() const {
        return *selection_manager_;
    }

    TextBoxCaretManager& CaretManager() const noexcept {
        return *caret_manager_;
    }
    
    TextBoxMouseInputHandler& MouseInputHandler() const {
        return *mouse_input_handler_;
    }

    TextBoxKeyboardInputHandler& KeyboardInputHandler() const {
        return *keyboard_input_handler_;
    }

    TextBoxEditor& Editor() const noexcept {
        return *editor_;
    }

    TextBoxIMEManager& IMEManager() const {
        return *ime_manager_;
    }

private:
    TextBox* owner_{};
    std::unique_ptr<TextBoxHitTestManager> hit_test_manager_;
    std::unique_ptr<TextBoxIndexManager> index_manager_;
    std::unique_ptr<TextBoxSelectionManager> selection_manager_;
    std::unique_ptr<TextBoxCaretManager> caret_manager_;
    std::unique_ptr<TextBoxMouseInputHandler> mouse_input_handler_;
    std::unique_ptr<TextBoxKeyboardInputHandler> keyboard_input_handler_;
    std::unique_ptr<TextBoxEditor> editor_;
    std::unique_ptr<TextBoxIMEManager> ime_manager_;
};

}
}