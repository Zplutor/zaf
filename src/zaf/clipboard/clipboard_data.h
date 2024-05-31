#pragma once

/**
@file
    Defines the class zaf::clipboard::ClipboardData.
*/

#include <zaf/clipboard/data_descriptor.h>
#include <zaf/clipboard/medium.h>

namespace zaf::clipboard {

/**
A base class representing a kind of data in the clipboard, providing an unified interface to read
and write clipboard data.
*/
class ClipboardData {
public:
    ClipboardData() = default;
    virtual ~ClipboardData() = default;

    /**
    Saves the data to a medium according to the specified data descriptor.

    @param data_descriptor
        The data descriptor describing how the data will be saved, including the format type to 
        save, and the desired medium types the method returns.

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
        2. Check if the target device specified in the data descriptor is supported by the data. If
           not, throw zaf::InvalidOperationError. In general, clipboard data that is 
           device-independent may ignore this attribute.
        3. Check if the aspect and aspect index specified in the data descriptor are supported by 
           the data. If not, throw zaf::InvalidOperationError. In general, most clipboard data 
           support DataAspect::Content only, and the aspect index is ignored.
        4. Choose the most appropriate medium type supported by the data from the medium types 
           specified in the data descriptor. If none of the types are supported, throw
           zaf::InvalidOperationError.
        5. Write the content of the data to the chosen medium in the specified format type and 
           according to other attributes. If the content is invalid, for example, if the content 
           is empty, throw zaf::InvalidDataError.
    */
    virtual Medium SaveToMedium(const DataDescriptor& data_descriptor) const = 0;

    /**
    Loads data from the specified medium according to the specified data descriptor.

    @param medium
        The medium from which the data will be loaded.

    @param data_descriptor
        The data descriptor describing how the data will be loaded, such as the format type to 
        load.

    @throw zaf::InvalidOperationError
        Thrown if the data cannot be loaded from the medium as specified by the data descriptor.
        This includes but is not limited to the following situations:
        - the type of the medium is not supported by the data;
        - the format type specified in the data descriptor is not compatible with the data.

    @throw zaf::InvalidDataError
        Thrown if the content to be loaded in the medium is invalid.

    @throw ...
        Any exceptions thrown by the overriding implementation.

    @details
        Overriding guide:
        1. Check if the data is compatible with the format type specified in the data descriptor.
           If not, throw zaf::InvalidOperationError.
        2. Check if the target device specified in the data descriptor is supported by the data. If
           not, throw zaf::InvalidOperationError. In general, clipboard data that is 
           device-independent may ignore this attribute.
        3. Check if the aspect and aspect index specified in the data descriptor are supported by 
           the data. If not, throw zaf::InvalidOperationError. In general, most clipboard data 
           support DataAspect::Content only, and the aspect index is ignored.
        4. Check if the type of the medium is supported by the data. If not, throw 
           zaf::InvalidOperationError. Note that the medium type specified in the data descriptor
           is not used when loading data.
        5. Read the content from the medium in the specified format type and according to other 
           attributes. If the content in medium is invalid, for example, if the content is empty, 
           throw zaf::InvalidDataError.
    */
    virtual void LoadFromMedium(const Medium& medium, const DataDescriptor& data_descriptor) = 0;
};

}