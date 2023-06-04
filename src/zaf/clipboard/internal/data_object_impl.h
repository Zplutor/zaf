#pragma once

#include <oleidl.h>
#include <zaf/clipboard/internal/format_item.h>

namespace zaf::clipboard::internal {

class DataObjectImpl : public IDataObject {
public:
    DataObjectImpl();

    //IUnknown interfaces
    HRESULT QueryInterface(REFIID riid, LPVOID* ppvObj) override;
    ULONG AddRef() override;
    ULONG Release() override;

    //IDataObject interfaces
    HRESULT GetData(FORMATETC* pformatetcIn, STGMEDIUM* pmedium) override;
    HRESULT GetDataHere(FORMATETC* pformatetc, STGMEDIUM* pmedium) override;
    HRESULT QueryGetData(FORMATETC* pformatetc) override;
    HRESULT GetCanonicalFormatEtc(FORMATETC* pformatectIn, FORMATETC* pformatetcOut) override;
    HRESULT SetData(FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease) override;
    HRESULT EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc) override;
    HRESULT DAdvise(
        FORMATETC* pformatetc,
        DWORD advf,
        IAdviseSink* pAdvSink,
        DWORD* pdwConnection) override;
    HRESULT DUnadvise(DWORD dwConnection) override;
    HRESULT EnumDAdvise(IEnumSTATDATA** ppenumAdvise) override;

private:
    LONG reference_count_{ 1 };
    std::shared_ptr<FormatItemList> format_items_;
};

}