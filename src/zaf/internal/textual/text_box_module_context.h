#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/text_box.h>
#include <zaf/graphic/text/text_layout.h>

namespace zaf {
namespace internal {

class TextBoxEditor;
class TextBoxHitTestManager;
class TextBoxInlineObjectManager;
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

    TextBoxInlineObjectManager& InlineObjectManager() const {
        return *inline_object_manager_;
    }

private:
    TextBox* owner_{};
    std::unique_ptr<TextBoxHitTestManager> hit_test_manager_;
    std::unique_ptr<TextBoxSelectionManager> selection_manager_;
    std::unique_ptr<TextBoxMouseInputHandler> mouse_input_handler_;
    std::unique_ptr<TextBoxKeyboardInputHandler> keyboard_input_handler_;
    std::unique_ptr<TextBoxEditor> editor_;
    std::unique_ptr<TextBoxInlineObjectManager> inline_object_manager_;
};

}
}