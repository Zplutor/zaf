#include <zaf/graphic/localized_strings.h>
#include <zaf/base/error/com_error.h>

namespace zaf {

std::wstring LocalizedStrings::GetLocaleName(std::size_t index) const {

    UINT32 length = 0;
    HRESULT result = Ptr()->GetLocaleNameLength(static_cast<UINT32>(index), &length);

    ZAF_THROW_IF_COM_ERROR(result);

    std::wstring locale_name(length + 1, 0);
    result = Ptr()->GetLocaleName(
        static_cast<UINT32>(index),
        &locale_name[0], 
        static_cast<UINT32>(locale_name.length()));

    ZAF_THROW_IF_COM_ERROR(result);

    locale_name.resize(length);
    return locale_name;
}


std::wstring LocalizedStrings::GetString(std::size_t index) const {

    UINT32 length = 0;
    HRESULT result = Ptr()->GetStringLength(static_cast<UINT32>(index), &length);

    ZAF_THROW_IF_COM_ERROR(result);

    std::wstring string(length + 1, 0);
    result = Ptr()->GetString(
        static_cast<UINT32>(index),
        &string[0],
        static_cast<UINT32>(string.length()));

    ZAF_THROW_IF_COM_ERROR(result);

    string.resize(length);
    return string;
}


std::optional<std::size_t> LocalizedStrings::FindLocaleName(const std::wstring& local_name) const {

    UINT32 index = 0;
    BOOL is_existent = FALSE;
    HRESULT result = Ptr()->FindLocaleName(local_name.c_str(), &index, &is_existent);

    ZAF_THROW_IF_COM_ERROR(result);

    if (is_existent) {
        return index;
    }
    else {
        return std::nullopt;
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