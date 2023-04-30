#pragma once

#include <functional>
#include <string>

namespace zaf {

class RichEdit;
class TextRange;

/**
 Prototype of text validator.

 @param rich_edit
    The rich edit that is being inputted.
    
 @param new_text
    The new text that is being validated.

 @return
    Return a value indicating that whether new_text can be inputted to rich_edit.
 */
typedef std::function<bool(const RichEdit& rich_edit, const std::wstring& new_text)> TextValidator;

/**
 Get a text validator that validates decimal number text.
 */
TextValidator GetNumberTextValidator();

}