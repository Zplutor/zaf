#pragma once

#include <dwrite.h>
#include <cstdint>
#include <optional>
#include <string>
#include <zaf/base/com_object.h>
#include <zaf/internal/enumerator.h>

namespace zaf {

/**
 Represents a collection of strings indexed by locale name.   
 */
class LocalizedStrings : public COMObject<IDWriteLocalizedStrings> {
public:
    typedef internal::ComContainerEnumerator<LocalizedStrings, std::pair<std::wstring, std::wstring>> Enumerator;
    typedef internal::ComContainerEnumerator<LocalizedStrings, std::wstring> StringEnumerator;

public:
    LocalizedStrings() { }
    explicit LocalizedStrings(IDWriteLocalizedStrings* handle) : COMObject(handle) { }

    /**
     Gets the number of language/string pairs.
     */
    std::size_t GetCount() const {
        return Inner()->GetCount();
    }

    /**
     Get the locale name at specified index.

     @param index
         Zero-based index of the locale name.

     @return 
         Return an empty string if failed.
     */
    std::wstring GetLocaleName(std::size_t index) const;

    /**
     Get the string at specified index.

     @param index
         Zero-based index of the string.

     @return 
         Return an empty string if failed.
     */
    std::wstring GetString(std::size_t index) const;

    /**
     Get the index of the item with the specified locale name.
    
     @param locale_name
         Locale name to look for.
    
     @return
         Return std::nullopt if the locale name is not found.
    */
    std::optional<std::size_t> FindLocaleName(const std::wstring& local_name) const;

    /**
     Get an enumerator for both locale names and strings.
     */
    Enumerator GetEnumerator() const;

    /**
     Get an enumerator for strings.
     */
    StringEnumerator GetStringEnumerator() const;
};

}