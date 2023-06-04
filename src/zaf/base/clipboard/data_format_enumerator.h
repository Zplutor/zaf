#pragma once

#include <oleidl.h>
#include <zaf/base/clipboard/internal/data_format_item.h>

namespace zaf {

class DataFormatEnumerator : public IEnumFORMATETC {
public:
    explicit DataFormatEnumerator(std::shared_ptr<internal::DataFormatItemList> format_items);

    //IUnknown interfaces
    HRESULT QueryInterface(REFIID riid, LPVOID* ppvObj) override;
    ULONG AddRef() override;
    ULONG Release() override;

    //IEnumFORMATETC interfaces
    HRESULT Next(ULONG celt, FORMATETC* rgelt, ULONG* pceltFetched) override;
    HRESULT Skip(ULONG celt) override;
    HRESULT Reset(void) override;
    HRESULT Clone(IEnumFORMATETC** ppenum) override;

private:
    LONG reference_count_{ 1 };
    std::size_t current_index_{};
    std::shared_ptr<internal::DataFormatItemList> format_items_;
};

}