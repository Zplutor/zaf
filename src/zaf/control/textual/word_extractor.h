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
    The index around which to extract the completed word range. It may exceed the length of the 
    text.

@return 
    A range of the completed word around the specified index.
*/
using WordExtractor = std::function<
    Range(std::wstring_view text, std::size_t index)
>;


/**
Gets the default word extractor used in text box.

@return
    The default word extractor.

@post
    The returned value is not null.

@details
    The following character categories are considered as a word respectively:
    - Alphabetic and numeric characters.
    - Blank characters, such as space and tab.
*/
WordExtractor DefaultWordExtractor() noexcept;

}