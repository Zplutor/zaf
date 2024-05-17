#include <zaf/clipboard/data_descriptor_enumerator.h>
#include <zaf/base/error/com_error.h>

namespace zaf::clipboard {

DataDescriptorEnumerator::DataDescriptorEnumerator(COMPtr<IEnumFORMATETC> inner) : 
    COMObject(std::move(inner)) {

}


std::optional<DataDescriptor> DataDescriptorEnumerator::Next() {

    FORMATETC inner{};
    HRESULT hresult = Inner()->Next(1, &inner, nullptr);
    ZAF_THROW_IF_COM_ERROR(hresult);

    if (hresult == S_OK) {
        return DataDescriptor{ inner };
    }
    return std::nullopt;
}


void DataDescriptorEnumerator::Reset() {

    HRESULT hresult = Inner()->Reset();
    ZAF_THROW_IF_COM_ERROR(hresult);
}

}