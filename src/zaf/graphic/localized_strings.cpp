#include <zaf/graphic/localized_strings.h>
#include <zaf/base/define.h>

namespace zaf {

std::wstring LocalizedStrings::GetLocaleName(std::size_t index, std::error_code& error_code) const {

    UINT32 length = 0;
    HRESULT result = GetHandle()->GetLocaleNameLength(index, &length);

    error_code = MakeComErrorCode(result);
    if (! IsSucceeded(error_code)) {
        return std::wstring();
    }

    std::wstring locale_name(length + 1, 0);
    result = GetHandle()->GetLocaleName(index, &locale_name[0], locale_name.length());

    error_code = MakeComErrorCode(result);
    if (IsSucceeded(error_code)) {
        locale_name.resize(length);
        return locale_name;
    }
    else {
        return std::wstring();
    }
}


std::wstring LocalizedStrings::GetString(std::size_t index, std::error_code& error_code) const {

    UINT32 length = 0;
    HRESULT result = GetHandle()->GetStringLength(index, &length);

    error_code = MakeComErrorCode(result);
    if (! IsSucceeded(error_code)) {
        return std::wstring();
    }

    std::wstring string(length + 1, 0);
    result = GetHandle()->GetString(index, &string[0], string.length());

    error_code = MakeComErrorCode(result);
    if (IsSucceeded(error_code)) {
        string.resize(length);
        return string;
    }
    else {
        return std::wstring();
    }
}


std::size_t LocalizedStrings::FindLocaleName(const std::wstring& local_name, std::error_code& error_code) const {

    UINT32 index = 0;
    BOOL is_existent = FALSE;
    HRESULT result = GetHandle()->FindLocaleName(local_name.c_str(), &index, &is_existent);

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


LocalizedStrings::Enumerator LocalizedStrings::GetEnumerator() const {

    return Enumerator(
        *this,
        [](const LocalizedStrings& localized_strings) { 
            return localized_strings.GetCount(); 
        },
        [](const LocalizedStrings& localized_strings, std::size_t index) {
            std::wstring locale_name = localized_strings.GetLocaleName(index);
            std::wstring string = localized_strings.GetString(index);
            return std::make_pair(locale_name, string);
        }
    );
}


LocalizedStrings::StringEnumerator LocalizedStrings::GetStringEnumerator() const {

    return StringEnumerator(
        *this,
        [](const LocalizedStrings& localized_strings) {
            return localized_strings.GetCount();
        },
        [](const LocalizedStrings& localized_strings, std::size_t index) {
            return localized_strings.GetString(index);
        }
    );
}

}