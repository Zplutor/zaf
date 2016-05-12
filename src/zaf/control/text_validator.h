#pragma once

#include <functional>
#include <string>

namespace zaf {

class TextBox;
class TextRange;

typedef std::function<bool(const TextBox&, const std::wstring&)> TextValidator; 

TextValidator GetNumberTextValidator();

}