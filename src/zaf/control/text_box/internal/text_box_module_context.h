#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/text_box.h>
#include <zaf/graphic/text/text_layout.h>

namespace zaf {
namespace internal {

class TextBoxEditor;
class TextBoxMouseInputHandler;
class TextBoxKeyboardInputHandler;
class TextBoxSelectionManager;

class TextBoxModuleContext : NonCopyableNonMovable {
public:
    explicit TextBoxModuleContext(TextBox* owner);

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
    std::unique_ptr<TextBoxSelectionManager> selection_manager_;
    std::unique_ptr<TextBoxMouseInputHandler> mouse_input_handler_;
    std::unique_ptr<TextBoxKeyboardInputHandler> keyboard_input_handler_;
    std::unique_ptr<TextBoxEditor> editor_;
};

}
}