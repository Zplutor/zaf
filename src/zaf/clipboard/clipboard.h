#pragma once

#include <string_view>
#include <zaf/clipboard/format_type.h>
#include <zaf/object/object_type.h>

namespace zaf::clipboard {

class Clipboard {
public:
    Clipboard() = delete;

    /**
    Gets unicode format text from clipboard.

    @return
        A string with unicode format.

    @throw zaf::Error
        Throws if fails to get unicode format text from clipboard.
    */
    static std::wstring GetText();

    /**
    Sets unicode format text to clipboard.

    @param text
        The text to be set to clipboard.

    @throw zaf::Error
        Throws if fails to set text to clipboard.
    */
    static void SetText(std::wstring_view text);

    /**
    Registers type of ClipboardData for specified format type.

    @param format_type
        The specified format type.

    @param clipboard_data_type
        The runtime type of ClipboardData. The class of the type must be default constructible. 
        When handling clipboard data with the specified format type, CreateInstance() of the type 
        will be called to create an instance, and then ClipboardData::LoadFromMedium() will be 
        called to load data.

    @throw std::logic_error
        clipboard_data_type is not a type derives from ClipboardData.

    The registered type will be used in clipboard data handling if the format type matches. A 
    typical situation is DataObject::GetData().
    */
    static void RegisterClipboardData(FormatType format_type, ObjectType* clipboard_data_type);

    /**
    Gets registered type of clipboard data for specified format type.

    @param format_type
        The specified format type.

    @return
        Returns the registered type of ClipboardData or nullptr if the specified format type is not
        found.

    Standard format types have been registered by default. Using this method can also get the 
    default clipboard data types for standard format types.
    */
    static ObjectType* GetRegisteredClipboardData(FormatType format_type) noexcept;
};

}