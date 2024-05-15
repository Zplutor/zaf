#pragma once

/**
@file
    Defines the class zaf::clipboard::ClipboardData.
*/

#include <zaf/base/non_copyable.h>
#include <zaf/clipboard/format.h>
#include <zaf/clipboard/medium.h>
#include <zaf/object/object.h>

namespace zaf::clipboard {

/**
A base class representing a kind of data in the clipboard, providing an unified interface to 
read and write clipboard data.

@details
    The clipboard functionality in zaf uses the dynamic type mechanism to read and write clipboard 
    data. To define a new kind of clipboard data, users can follow these steps:

    1. Define a new class that inherits from ClipboardData, use ZAF_DECLARE_TYPE and 
       ZAF_DEFINE_TYPE to enable dynamic type support for the class.
       @code{.cpp}
       //In .h
       class MyClipboardData : public zaf::clipboard::ClipboardData {
       public:
           ZAF_DECLARE_TYPE;
       };

       //In .cpp
       ZAF_DEFINE_TYPE(ClipboardData)
       ZAF_DEFINE_TYPE_END;
       @endcode

    2. Override ClipboardData::SaveToMedium() and ClipboardData::LoadFromMedium() to write and read
       the specific data in the new class.

    3. Register the new clipboard data with a new format type.
       @code{.cpp}
       //Create a format type for the new clipboard data.
       zaf::clipboard::FormatType my_format_type = zaf::clipboard::MakePrivateFormatType(1);

       //Register the new clipoard data with its type.
       zaf::clipboard::Clipboard::RegisterClipboardData(my_format_type, MyClipboardData::Type);
       @endcode
*/
class ClipboardData : public Object, NonCopyableNonMovable {
public:
    ZAF_DECLARE_TYPE;

public:
    ClipboardData() = default;
    virtual ~ClipboardData() = default;

    /**
    Saves the data to a medium in the specified format.

    @param format
        The format specifies the format type in which the data will be saved, and the desired
        medium type.

    @return
        The medium containing the data in the specified format. The type of the medium is identical
        to the medium type of the format.

    @throw zaf::InvalidOperationError
        Thrown if the specified format, either its format type or medium type, is not supported by
        the clipboard data. 

    @throw zaf::InvalidDataError
        Thrown if the clipboard data itself is invalid.

    @throw ...
        Any exceptions thrown by the overriding implementation.
    */
    virtual Medium SaveToMedium(const Format& format) = 0;

    /**
    Loads data from the specified meidum in the specified format type.

    @param format_type
        The format type in which the data will be loaded.

    @param medium
        The medium from which the data will be loaded.

    @throw zaf::InvalidOperationError
        Thrown if the specified format type or the specified medium type is not supported by the 
        clipboard data. 

    @throw zaf::InvalidDataError
        Thrown if the data in the medium is invalid.

    @throw ...
        Any exceptions thrown by the overriding implementation.
    */
    virtual void LoadFromMedium(FormatType format_type, const Medium& medium) = 0;
};

}