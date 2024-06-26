#include <zaf/internal/textual/text_box_module_context.h>
#include <zaf/internal/textual/text_box_caret_manager.h>
#include <zaf/internal/textual/text_box_editor.h>
#include <zaf/internal/textual/text_box_hit_test_manager.h>
#include <zaf/internal/textual/text_box_ime_manager.h>
#include <zaf/internal/textual/text_box_index_manager.h>
#include <zaf/internal/textual/text_box_mouse_input_handler.h>
#include <zaf/internal/textual/text_box_keyboard_input_handler.h>
#include <zaf/internal/textual/text_box_selection_manager.h>

namespace zaf::internal {

TextBoxModuleContext::TextBoxModuleContext(TextBox* owner) :
    owner_(owner),
    hit_test_manager_(std::make_unique<TextBoxHitTestManager>(this)),
    index_manager_(std::make_unique<TextBoxIndexManager>(this)),
    selection_manager_(std::make_unique<TextBoxSelectionManager>(this)),
    caret_manager_(std::make_unique<TextBoxCaretManager>(this)),
    mouse_input_handler_(std::make_unique<TextBoxMouseInputHandler>(this)),
    keyboard_input_handler_(std::make_unique<TextBoxKeyboardInputHandler>(this)),
    editor_(std::make_unique<TextBoxEditor>(this)),
    ime_manager_(std::make_unique<TextBoxIMEManager>(this)) {

}


TextBoxModuleContext::~TextBoxModuleContext() {

}


void TextBoxModuleContext::Initialize() {

    hit_test_manager_->Initialize();
    index_manager_->Initialize();
    selection_manager_->Initialize();
    caret_manager_->Initialize();
    mouse_input_handler_->Initialize();
    keyboard_input_handler_->Initialize();
    editor_->Initialize();
    ime_manager_->Initialize();
}

}

