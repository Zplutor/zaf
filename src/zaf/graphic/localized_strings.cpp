#include <zaf/graphic/localized_strings.h>

namespace zaf {

const std::wstring LocalizedStrings::GetLocaleName(std::uint32_t index, std::error_code& error_code) const {

    std::uint32_t length = 0;
    HRESULT result = handle_->GetLocaleNameLength(index, &length);

    error_code = MakeComErrorCode(result);
    if (! IsSucceeded(error_code)) {
        return std::wstring();
    }

    std::wstring locale_name(length + 1, 0);
    result = handle_->GetLocaleName(index, &locale_name[0], locale_name.length());

    error_code = MakeComErrorCode(result);
    if (IsSucceeded(error_code)) {
        locale_name.resize(length);
        return locale_name;
    }
    else {
        return std::wstring();
    }
}


const std::wstring LocalizedStrings::GetString(std::uint32_t index, std::error_code& error_code) const {

    std::uint32_t length = 0;
    HRESULT result = handle_->GetStringLength(index, &length);

    error_code = MakeComErrorCode(result);
    if (! IsSucceeded(error_code)) {
        return std::wstring();
    }

    std::wstring string(length + 1, 0);
    result = handle_->GetString(index, &string[0], string.length());

    error_code = MakeComErrorCode(result);
    if (IsSucceeded(error_code)) {
        string.resize(length);
        return string;
    }
    else {
        return std::wstring();
    }
}

}