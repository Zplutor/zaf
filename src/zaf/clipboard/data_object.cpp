#include <zaf/clipboard/data_object.h>
#include <zaf/base/as.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/clipboard/data_descriptor.h>
#include <zaf/clipboard/internal/data_object_impl.h>
#include <zaf/clipboard/drop_files_data.h>
#include <zaf/clipboard/text_data.h>
#include <zaf/clipboard/unknown_data.h>
#include <zaf/base/error/com_error.h>

namespace zaf::clipboard {

DataObject DataObject::Create() {
    return DataObject{ MakeCOMPtr<internal::DataObjectImpl>() };
}


DataDescriptorEnumerator DataObject::EnumerateDataDescriptors() const {

    COMPtr<IEnumFORMATETC> enumerator_inner;
    HRESULT hresult = Inner()->EnumFormatEtc(DATADIR_GET, enumerator_inner.Reset());
    ZAF_THROW_IF_COM_ERROR(hresult);

    return DataDescriptorEnumerator{ enumerator_inner };
}


std::shared_ptr<ClipboardData> DataObject::GetData(const DataDescriptor& data_descriptor) const {

    std::shared_ptr<ClipboardData> result;

    auto data_type = Clipboard::GetRegisteredClipboardData(data_descriptor.FormatType());
    if (data_type) {
        result = As<ClipboardData>(data_type->CreateInstance());
    }
    else {
        result = std::make_shared<UnknownData>();
    }

    InnerGetData(data_descriptor, *result);
    return result;
}


void DataObject::InnerGetData(const DataDescriptor& data_descriptor, ClipboardData& data) const {

    auto format_inner = data_descriptor.Inner();

    STGMEDIUM medium_inner{};
    HRESULT hresult = Inner()->GetData(&format_inner, &medium_inner);
    ZAF_THROW_IF_COM_ERROR(hresult);

    data.LoadFromMedium(Medium{ medium_inner }, data_descriptor);
}


std::wstring DataObject::GetText() const {

    TextData text_data;
    InnerGetData(DataDescriptor::FromFormatType(FormatType::Text), text_data);
    return text_data.DetachText();
}


void DataObject::SetData(
    std::shared_ptr<ClipboardData> data,
    const DataDescriptor& data_descriptor) {

    InnerSetData(data_descriptor, *data);
}


void DataObject::InnerSetData(const DataDescriptor& data_descriptor, ClipboardData& data) {

    auto medium = data.SaveToMedium(data_descriptor);
    auto medium_inner = medium.Inner();

    auto format_inner = data_descriptor.Inner();

    HRESULT hresult = Inner()->SetData(
        &format_inner,
        &medium_inner,
        TRUE);

    ZAF_THROW_IF_COM_ERROR(hresult);

    //Owner ship of medium has been transfered to IDataObject, so detach it from Medium.
    auto detached = medium.Detach();
}


void DataObject::SetText(std::wstring text) {

    TextData text_data{ std::move(text) };
    InnerSetData(DataDescriptor::FromFormatType(FormatType::Text), text_data);
}

}