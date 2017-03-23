#include "logic/data_generating/string_generating.h"
#include <cctype>
#include "logic/data_generating/random.h"

static std::wstring GenerateWord();

std::wstring GenerateNaturalString(std::size_t max_word_count) {

    std::wstring string;

    std::size_t word_count = GenerateRandomInteger<std::size_t>(1, max_word_count);
    for (std::size_t count = 0; count < word_count; ++count) {

        auto word = GenerateWord();
        if (count == 0) {
            word[0] = std::toupper(word[0]);
        }

        string.append(word);

        if (count != word_count - 1) {
            string.append(1, ' ');
        }
    }

    return string;
}


static std::wstring GenerateWord() {

    static const wchar_t letters[] = L"abcdefghijklmnopqrstuvwxyz"; 
    const std::size_t letters_count = std::distance(std::begin(letters), std::end(letters));

    std::wstring word; 
    std::size_t word_letter_count = GenerateRandomInteger<std::size_t>(2, 10);
    
    for (std::size_t count = 0; count < word_letter_count; ++count) {

        std::size_t index = GenerateRandomInteger<std::size_t>(0, letters_count);
        word.append(1, letters[index]);
    }

    return word;
}