#include <zaf/clipboard/data_object.h>
#include <zaf/base/as.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/clipboard/data_descriptor.h>
#include <zaf/clipboard/internal/data_object_impl.h>
#include <zaf/clipboard/drop_files_data.h>
#include <zaf/clipboard/text_data.h>
#include <zaf/base/error/com_error.h>

namespace zaf::clipboard {

DataObject DataObject::Create() {
    return DataObject{ MakeCOMPtr<internal::DataObjectImpl>() };
}


DataDescriptorEnumerator DataObject::EnumerateDataDescriptors() const {

    COMPtr<IEnumFORMATETC> enumerator_inner;
    HRESULT hresult = Ptr()->EnumFormatEtc(DATADIR_GET, enumerator_inner.Reset());
    ZAF_THROW_IF_COM_ERROR(hresult);

    return DataDescriptorEnumerator{ enumerator_inner };
}


void DataObject::GetData(const DataDescriptor& data_descriptor, ClipboardData& data) const {

    auto medium = GetData(data_descriptor);
    data.ReadFromMedium(medium, data_descriptor);
}


Medium DataObject::GetData(const DataDescriptor& data_descriptor) const {

    auto descriptor_inner = data_descriptor.Inner();

    STGMEDIUM medium_inner{};
    HRESULT hresult = Ptr()->GetData(&descriptor_inner, &medium_inner);
    ZAF_THROW_IF_COM_ERROR(hresult);

    return Medium{ medium_inner };
}


void DataObject::SetData(const DataDescriptor& data_descriptor, const ClipboardData& data) {
    SetData(data_descriptor, data.WriteToMedium(data_descriptor));
}


void DataObject::SetData(const DataDescriptor& data_descriptor, Medium medium) {

    auto descriptor_inner = data_descriptor.Inner();
    auto medium_inner = medium.Inner();

    //fRelease is set to TRUE to transfer the ownership of the medium to the data object.
    HRESULT hresult = Ptr()->SetData(&descriptor_inner, &medium_inner, TRUE);
    ZAF_THROW_IF_COM_ERROR(hresult);

    //Detach the medium as its ownership has been transfered.
    auto detached = medium.Detach();
}


std::wstring DataObject::GetText() const {

    TextData text_data;
    GetData(DataDescriptor::FromFormatType(FormatType::Text), text_data);
    return text_data.Detach();
}


void DataObject::SetText(std::wstring text) {
    SetData(DataDescriptor::FromFormatType(FormatType::Text), TextData{ std::move(text) });
}

}