#pragma once

#include <functional>
#include <string>

namespace zaf {

class TextBox;
class TextRange;

/**
 Prototype of text validator.

 @param text_box
    The text box that is being inputted.
    
 @param new_text
    The new text that is being validated.

 @return
    Return a value indicating that whether new_text can be inputted to text_box.
 */
typedef std::function<bool(const TextBox& text_box, const std::wstring& new_text)> TextValidator;

/**
 Get a text validator that validates decimal number text.
 */
TextValidator GetNumberTextValidator();

}