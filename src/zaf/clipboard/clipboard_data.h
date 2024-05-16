#pragma once

/**
@file
    Defines the class zaf::clipboard::ClipboardData.
*/

#include <zaf/base/non_copyable.h>
#include <zaf/clipboard/data_descriptor.h>
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
    Saves the data to a medium according to the specified data descriptor.

    @param data_descriptor
        The data descriptor describing the format type in which the data will be saved and the 
        desired medium types the method returns.

    @return
        The medium containing the data in the specified format. The type of the medium is one of 
        the types specified in the data descriptor.

    @throw zaf::InvalidOperationError
        Thrown if the data cannot be saved as specified by the data descriptor. This includes but 
        is not limited to the following situations:
        - the format type specified in the data descriptor is not compatible with the data;
        - none of the meidum types specified in the data descriptor are supported by the data.

    @throw zaf::InvalidDataError
        Thrown if the content of the data is invalid.

    @throw ...
        Any exceptions thrown by the overriding implementation.

    @details
        Overriding guide:
        1. Check if the data is compatible with the format type specified in the data descriptor.
           If not, throw zaf::InvalidOperationError.
        2. Choose the most appropriate medium type supported by the data from the medium types 
           specified in the data descriptor. If none of the types are supported, throw
           zaf::InvalidOperationError.
        3. Check other attributes in the FORMATETC structure within the data descriptor as needed,
           and throw zaf::InvalidOperationError if the check fails. These attributes are not often 
           used and can be ignored in most cases.
        4. Write the content of the data to the chosen medium in the specified format type. If the 
           data is invalid, for example, if the content is empty, throw zaf::InvalidDataError.
    */
    virtual Medium SaveToMedium(const DataDescriptor& data_descriptor) = 0;

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