#pragma once

#include <functional>
#include <string_view>
#include <zaf/base/range.h>

namespace zaf::textual {

/**
A WordExtractor is used to extract a range of the completed word around the index in text.

@param text 
    The input text from which to extract the word range.

@param index 
    The index around which to extract the completed word range. It may exceed the lenght of the 
    text.

@return 
    A range of the completed word around the specified index.
*/
using WordExtractor = std::function<
    Range(std::wstring_view text, std::size_t index)
>;


/**
Returns the default word extractor used in text box.
*/
WordExtractor DefaultWordExtractor() noexcept;

}