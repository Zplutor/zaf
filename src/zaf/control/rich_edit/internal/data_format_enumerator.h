#pragma once

#include <oleidl.h>

namespace zaf::rich_edit::internal {

class DataFormatEnumerator : public IEnumFORMATETC {
public:
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
    std::size_t count_{};
    std::size_t index_{};
};

}