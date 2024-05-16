#pragma once

/**
@file 
    Defines the zaf::clipboard::Clipboard class.
*/

#include <string_view>
#include <zaf/clipboard/data_object.h>
#include <zaf/clipboard/format_type.h>
#include <zaf/object/object_type.h>

namespace zaf::clipboard {

/**
Represents the system clipboard.

@details
    Since the system clipboard is a unique singleton, Clipboard is not allowed to be instantiated,
    and all of its methods are static.
*/
class Clipboard {
public:
    /**
    A deleted default constructor to forbid instantiation.
    */
    Clipboard() = delete;

    /**
    Gets unicode text from the clipboard.

    @return
        The unicode text in the clipboard.

    @throw zaf::Win32Error
        Thrown if fails to get unicode text from the clipboard.
    */
    static std::wstring GetText();

    /**
    Sets the specified unicode text to the clipboard.

    @param text
        The text to be set to the clipboard.

    @throw zaf::Win32Error
        Thrown if fails to set text to the clipboard.

    @details
        The text will be set as a CF_UNICODETEXT format data.
    */
    static void SetText(std::wstring_view text);

    /**
    Gets a data object containing the content stored in the clipboard.

    @return
        The data object got from the clipboard.

    @throw zaf::COMError
        Thrown if fails to get the data object.

    @details
        This method uses OleGetClipboard() to retrieve the data object.
    */
    static DataObject GetDataObject();

    /**
    Registers the specified clipboard data type for the specified format type.

    @param format_type
        The format type to which the clipboard data type will be registered.

    @param clipboard_data_type
        The clipboard data type to be registered, whose class must inherit from ClipboardData and 
        must be default constructible.
        
    @pre
        clipboard_data_type is not null.

    @throw zaf::PreconditionError
        Thrown if the precondition is violated.

    @throw zaf::InvalidTypeError
        Thrown if the class of clipboard_data_type doesn't inherit from ClipboardData.

    @details
        The registered clipboard data type will be used in clipboard data handling if the format 
        type matches. A typical situation is when calling clipboard::DataObject::GetData() with a 
        specified format type, ObjectType::CreateInstance() of the matched clipboard data type 
        will be called to create an instance, and then clipboard::ClipboardData::LoadFromMedium() 
        of the instance will be called to load data to the instance.
    */
    static void RegisterClipboardData(FormatType format_type, ObjectType* clipboard_data_type);

    /**
    Gets the registered clipboard data type for the specified format type.

    @param format_type
        The format type to find.

    @return
        The registered clipboard data type. It will be null if no matched clipboard data type is 
        found.

    @details
        Standard format types have been registered by default. Using this method can also get the 
        default clipboard data types for standard format types.
    */
    static ObjectType* GetRegisteredClipboardData(FormatType format_type) noexcept;
};

}