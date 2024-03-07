#include <zaf/internal/textual/text_box_module_context.h>
#include <zaf/internal/textual/text_box_editor.h>
#include <zaf/internal/textual/text_box_mouse_input_handler.h>
#include <zaf/internal/textual/text_box_keyboard_input_handler.h>
#include <zaf/internal/textual/text_box_selection_manager.h>

namespace zaf::internal {

TextBoxModuleContext::TextBoxModuleContext(TextBox* owner) :
    owner_(owner),
    selection_manager_(std::make_unique<TextBoxSelectionManager>(this)),
    mouse_input_handler_(std::make_unique<TextBoxMouseInputHandler>(this)),
    keyboard_input_handler_(std::make_unique<TextBoxKeyboardInputHandler>(this)),
    editor_(std::make_unique<TextBoxEditor>(this)) {

}


void TextBoxModuleContext::Initialize() {

    selection_manager_->Initialize();
    mouse_input_handler_->Initialize();
    keyboard_input_handler_->Initialize();
    editor_->Initialize();
}

}

