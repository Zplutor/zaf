#include <zaf/clipboard/format_enumerator.h>
#include <zaf/base/error/com_error.h>

namespace zaf::clipboard {

FormatEnumerator::FormatEnumerator(COMPtr<IEnumFORMATETC> inner) : COMObject(std::move(inner)) {

}


std::optional<DataDescriptor> FormatEnumerator::Next() {

    FORMATETC inner{};
    HRESULT hresult = Inner()->Next(1, &inner, nullptr);
    ZAF_THROW_IF_COM_ERROR(hresult);

    if (hresult == S_OK) {
        return DataDescriptor{ inner };
    }
    return std::nullopt;
}


void FormatEnumerator::Reset() {

    HRESULT hresult = Inner()->Reset();
    ZAF_THROW_IF_COM_ERROR(hresult);
}

}