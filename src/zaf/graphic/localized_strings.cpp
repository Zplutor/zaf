#include <zaf/graphic/localized_strings.h>
#include <zaf/base/define.h>

namespace zaf {

const std::wstring LocalizedStrings::GetLocaleName(std::size_t index, std::error_code& error_code) const {

    UINT32 length = 0;
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


const std::wstring LocalizedStrings::GetString(std::size_t index, std::error_code& error_code) const {

    UINT32 length = 0;
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


std::size_t LocalizedStrings::FindLocaleName(const std::wstring& local_name, std::error_code& error_code) {

    UINT32 index = 0;
    BOOL is_existent = FALSE;
    HRESULT result = handle_->FindLocaleName(local_name.c_str(), &index, &is_existent);

    error_code = MakeComErrorCode(result);
    if (! IsSucceeded(error_code)) {
        return InvalidIndex;
    }

    if (is_existent) {
        return index;
    }
    else {
        return InvalidIndex;
    }
}

}