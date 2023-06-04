#pragma once

#include <ObjIdl.h>
#include <zaf/base/clipboard/clipboard_data.h>
#include <zaf/base/com_object.h>

namespace zaf {

class DataObject {
public:
    DataObject();
    explicit DataObject(COMObject<IDataObject> inner);

    std::shared_ptr<ClipboardData> GetData(DataFormatID format_id) const;
    std::wstring GetText() const;

    void SetData(DataFormatID format_id, std::shared_ptr<ClipboardData> data);

    const COMObject<IDataObject>& Inner() const {
        return inner_;
    }

private:
    void InnerGetData(DataFormatID format_id, ClipboardData& data) const;

private:
    COMObject<IDataObject> inner_;
};

}