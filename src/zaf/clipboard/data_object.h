#pragma once

#include <ObjIdl.h>
#include <zaf/base/com_ptr.h>
#include <zaf/clipboard/clipboard_data.h>
#include <zaf/clipboard/format_enumerator.h>
#include <zaf/clipboard/format_type.h>

namespace zaf::clipboard {

class DataObject {
public:
    DataObject();
    explicit DataObject(COMPtr<IDataObject> inner);

    FormatEnumerator EnumerateFormats() const;

    std::shared_ptr<ClipboardData> GetData(FormatType format_type) const;
    std::wstring GetText() const;

    void SetData(FormatType format_type, std::shared_ptr<ClipboardData> data);
    void SetText(std::wstring text);

    const COMPtr<IDataObject>& Inner() const {
        return inner_;
    }

private:
    void InnerGetData(FormatType format_type, ClipboardData& data) const;
    void InnerSetData(FormatType format_type, ClipboardData& data);

private:
    COMPtr<IDataObject> inner_;
};

}