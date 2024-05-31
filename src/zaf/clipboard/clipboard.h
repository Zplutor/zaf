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
    Gets a data object containing the data stored in the clipboard.

    @return
        The data object got from the clipboard.

    @throw zaf::COMError
        Thrown if fails to get the data object.

    @details
        This method uses OleGetClipboard() to retrieve the data object.
    */
    static DataObject GetDataObject();

    /**
    Sets data in the specified data object to the clipboard.

    @param data_object
        The data object to set to the clipboard. It can be null, in which case the clipboard is 
        emptied.

    @throw zaf::COMError
        Thrown if fails to set the data object.

    @details
        This method uses OleSetClipboard() to modify the clipboard.
    */
    static void SetDataObject(const DataObject& data_object);
};

}