#pragma once

/**
@file
    Defines the class zaf::clipboard::DataObject.
*/

#include <ObjIdl.h>
#include <zaf/base/com_object.h>
#include <zaf/clipboard/clipboard_data.h>
#include <zaf/clipboard/data_descriptor_enumerator.h>
#include <zaf/clipboard/format_type.h>

namespace zaf::clipboard {

/**
An encapsulation class for the IDataObject interface.
*/
class DataObject : public COMObject<IDataObject> {
public:
    /**
    Creates a valid DataObject.

    @return
        Returns a valid DataObject.

    DataObjects created with the default constructor is invalid and unusable. Use this method to 
    create a valid DataObject.
    */
    static DataObject Create();
    
public:
    using COMObject::COMObject;

    DataDescriptorEnumerator EnumerateDataDescriptors() const;

    void GetData(const DataDescriptor& data_descriptor, ClipboardData& data) const;
    Medium GetData(const DataDescriptor& data_descriptor) const;

    void SetData(const DataDescriptor& data_descriptor, const ClipboardData& data);
    void SetData(const DataDescriptor& data_descriptor, Medium medium);

    std::wstring GetText() const;
    void SetText(std::wstring text);
};

}