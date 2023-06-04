#pragma once

#include <oleidl.h>
#include <zaf/base/clipboard/internal/data_format_item.h>

namespace zaf {

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
    friend class DataObject;

    void SetFormatData(const DataFormat& format, std::shared_ptr<ClipboardData> data);

private:
    LONG reference_count_{ 1 };
    std::shared_ptr<internal::DataFormatItemList> format_items_;
};

}