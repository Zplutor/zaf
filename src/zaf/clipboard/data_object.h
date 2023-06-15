#pragma once

#include <ObjIdl.h>
#include <zaf/base/com_object.h>
#include <zaf/clipboard/clipboard_data.h>
#include <zaf/clipboard/format_enumerator.h>
#include <zaf/clipboard/format_type.h>

namespace zaf::clipboard {

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

    FormatEnumerator EnumerateFormats() const;

    std::shared_ptr<ClipboardData> GetData(FormatType format_type) const;
    std::wstring GetText() const;

    void SetData(FormatType format_type, std::shared_ptr<ClipboardData> data);
    void SetText(std::wstring text);

private:
    void InnerGetData(FormatType format_type, ClipboardData& data) const;
    void InnerSetData(FormatType format_type, ClipboardData& data);
};

}