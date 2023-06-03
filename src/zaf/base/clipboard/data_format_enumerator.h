#pragma once

#include <oleidl.h>
#include <zaf/base/clipboard/data_format.h>

namespace zaf {

class DataFormatEnumerator : public IEnumFORMATETC {
public:
    explicit DataFormatEnumerator(std::shared_ptr<DataFormatList> formats);

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
    std::shared_ptr<DataFormatList> formats_;
    std::size_t current_index_{};
};

}