#include <zaf/base/clipboard/data_object.h>
#include <zaf/base/as.h>
#include <zaf/base/clipboard/data_object_impl.h>
#include <zaf/base/clipboard/text_data.h>
#include <zaf/base/clipboard/unknown_data.h>
#include <zaf/base/error/com_error.h>

namespace zaf {

DataObject::DataObject() : inner_(MakeCOMObject<DataObjectImpl>()) {

}


DataObject::DataObject(COMObject<IDataObject> inner) : inner_(std::move(inner)) {

}


std::shared_ptr<ClipboardData> DataObject::GetData(DataFormatID format_id) const {

    auto result = [format_id]() -> std::shared_ptr<ClipboardData> {
        if ((WORD)format_id == DataFormatID::Text) {
            return std::make_shared<TextData>();
        }
        return std::make_shared<UnknownData>();
    }();

    InnerGetData(format_id, *result);
    return result;
}


void DataObject::InnerGetData(DataFormatID format_id, ClipboardData& data) const {

    DataFormat format{ format_id };
    STGMEDIUM medium_inner{};
    HRESULT hresult = inner_->GetData(const_cast<FORMATETC*>(&format.Inner()), &medium_inner);
    ZAF_THROW_IF_COM_ERROR(hresult);

    data.LoadFromMedium(format, DataMedium{ medium_inner });
}


std::wstring DataObject::GetText() const {

    TextData text_data;
    InnerGetData((DataFormatID)DataFormatID::Text, text_data);
    return std::wstring{ text_data.Text() };
}


void DataObject::SetData(DataFormatID format_id, std::shared_ptr<ClipboardData> data) {


}

}