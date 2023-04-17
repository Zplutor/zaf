#include <Windows.h>
#include <WindowsX.h>
#include <richedit.h>
#include <richole.h>
#include <atlbase.h>
#include <atlctl.h>
#include <fstream>
#include <charconv>
#include <zaf/application.h>
#include <zaf/base/log.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/container/utility/range.h>
#include <zaf/base/stream.h>
#include <zaf/base/string/to_string.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include <zaf/window/dialog.h>
#include <zaf/control/button.h>
#include <zaf/control/text_box.h>
#include <zaf/window/message/hit_test_result.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/image/image.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/dpi.h>
#include <zaf/control/list_box.h>
#include <zaf/object/object_type.h>
#include <zaf/object/internal/reflection_manager.h>
#include <zaf/object/object_property.h>
#include <zaf/object/parsing/helpers.h>
#include <zaf/object/parsing/xaml_reader.h>
#include <zaf/object/creation.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/label.h>
#include <zaf/control/image_box.h>
#include <zaf/control/linear_box.h>
#include <zaf/base/registry/registry.h>
#include <zaf/base/error/error.h>
#include <zaf/object/type_definition.h>
#include <zaf/resource/resource_factory.h>
#include <zaf/object/boxing/string.h>
#include <zaf/object/boxing/boxing.h>
#include <zaf/control/combo_box.h>
#include <zaf/control/tree_control.h>
#include <zaf/control/tree_data_source.h>
#include <zaf/control/tree_control_delegate.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scroll_bar_thumb.h>
#include <zaf/control/check_box.h>
#include <zaf/rx/creation.h>
#include <zaf/rx/cancel.h>
#include <zaf/rx/scheduler.h>
#include <zaf/rx/timer.h>
#include <zaf/object/internal/property_helper.h>
#include <zaf/control/property_grid.h>
#include <zaf/window/popup_menu.h>
#include <zaf/control/menu_separator.h>

// {E16F8ACD-5B3A-4167-A449-DC570DD44459}
static const GUID MyOLEObjectID =
{ 0xe16f8acd, 0x5b3a, 0x4167, { 0xa4, 0x49, 0xdc, 0x57, 0xd, 0xd4, 0x44, 0x59 } };


class MyOLEObject : 
    public IOleObject, 
    public IViewObject {

public:
    HRESULT QueryInterface(REFIID riid, LPVOID* ppvObj) override {

        if (!ppvObj)
            return E_INVALIDARG;

        *ppvObj = NULL;

        if (riid == IID_IUnknown) {
            *ppvObj = (IOleObject*)this;
            AddRef();
            return S_OK;
        }

        if (riid == IID_IOleObject) {
            *ppvObj = (IOleObject*)this;
            AddRef();
            return S_OK;
        }

        if (riid == IID_IViewObject) {
            *ppvObj = (IViewObject*)this;
            AddRef();
            return S_OK;
        }

        if (riid == IID_IOleWindow) {
            *ppvObj = (IOleWindow*)this;
            AddRef();
            return S_OK;
        }

        if (riid == IID_IOleInPlaceObject) {
            *ppvObj = (IOleInPlaceObject*)this;
            AddRef();
            return S_OK;
        }

        if (riid == IID_IOleInPlaceObjectWindowless) {
            *ppvObj = (IOleInPlaceObjectWindowless*)this;
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    ULONG AddRef() override {
        InterlockedIncrement(&m_cRef);
        return m_cRef;
    }

    ULONG Release() override {
        ULONG ulRefCount = InterlockedDecrement(&m_cRef);
        if (0 == m_cRef) {
            delete this;
        }
        return ulRefCount;
    }

    HRESULT Draw(
        DWORD dwDrawAspect,
        LONG lindex,
        void* pvAspect,
        DVTARGETDEVICE* ptd,
        HDC hdcTargetDev,
        HDC hdcDraw,
        LPCRECTL lprcBounds,
        LPCRECTL lprcWBounds,
        BOOL(STDMETHODCALLTYPE* pfnContinue)(ULONG_PTR dwContinue),
        ULONG_PTR dwContinue) override {

        if (dwDrawAspect != DVASPECT_CONTENT) {
            return E_NOTIMPL;
        }

        RECT rect{};
        rect.left = lprcBounds->left;
        rect.top = lprcBounds->top;
        rect.right = lprcBounds->right;
        rect.bottom = lprcBounds->bottom;

        FillRect(hdcDraw, &rect, GetStockBrush(BLACK_BRUSH));

        return S_OK;
    }

    HRESULT GetColorSet(
        DWORD dwDrawAspect,
        LONG lindex,
        void* pvAspect,
        DVTARGETDEVICE* ptd,
        HDC hicTargetDev,
        LOGPALETTE** ppColorSet) override {
        return E_NOTIMPL;
    }

    HRESULT Freeze(
        DWORD dwDrawAspect,
        LONG lindex,
        void* pvAspect,
        DWORD* pdwFreeze) override {
        return E_NOTIMPL;
    }

    HRESULT Unfreeze(DWORD dwFreeze) override {
        return E_NOTIMPL;
    }

    HRESULT SetAdvise(
        DWORD aspects,
        DWORD advf,
        IAdviseSink* pAdvSink) override {
        return E_NOTIMPL;
    }

    HRESULT GetAdvise(
        DWORD* pAspects,
        DWORD* pAdvf,
        IAdviseSink** ppAdvSink) override {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SetClientSite(IOleClientSite* pClientSite) override {

        //client_site_ = pClientSite;
        return E_NOTIMPL;
    }

    HRESULT GetClientSite(IOleClientSite** ppClientSite) override {
        return E_NOTIMPL;
    }

    HRESULT SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj) override {
        return E_NOTIMPL;
    }

    HRESULT Close(DWORD dwSaveOption) override {
        return E_NOTIMPL;
    }

    HRESULT SetMoniker(DWORD dwWhichMoniker, IMoniker* pmk) override {
        return E_NOTIMPL;
    }

    HRESULT GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk) override {
        return E_NOTIMPL;
    }

    HRESULT InitFromData(IDataObject* pDataObject, BOOL fCreation, DWORD dwReserved) override {
        return E_NOTIMPL;
    }

    HRESULT GetClipboardData(DWORD dwReserved, IDataObject** ppDataObject) override {
        return E_NOTIMPL;
    }

    HRESULT DoVerb(
        LONG iVerb,
        LPMSG lpmsg,
        IOleClientSite* pActiveSite,
        LONG lindex,
        HWND hwndParent,
        LPCRECT lprcPosRect) override {
        return E_NOTIMPL;
    }

    HRESULT EnumVerbs(IEnumOLEVERB** ppEnumOleVerb) override {
        return E_NOTIMPL;
    }

    HRESULT Update(void) override {
        return E_NOTIMPL;
    }

    HRESULT IsUpToDate(void) override {
        return E_NOTIMPL;
    }

    HRESULT GetUserClassID(CLSID* pClsid) override {
        return E_NOTIMPL;
    }

    HRESULT GetUserType(DWORD dwFormOfType, LPOLESTR* pszUserType) override {
        return E_NOTIMPL;
    }

    HRESULT SetExtent(DWORD dwDrawAspect, SIZEL* psizel) override {
        return E_NOTIMPL;
    }

    HRESULT GetExtent(DWORD dwDrawAspect, SIZEL* psizel) override {
        return E_NOTIMPL;
    }

    HRESULT Advise(IAdviseSink* pAdvSink, DWORD* pdwConnection) override {
        return S_OK;
    }

    HRESULT Unadvise(DWORD dwConnection) override {
        return E_NOTIMPL;
    }

    HRESULT EnumAdvise(IEnumSTATDATA** ppenumAdvise) override {
        return E_NOTIMPL;
    }

    HRESULT GetMiscStatus(DWORD dwAspect, DWORD* pdwStatus) override {
        return E_NOTIMPL;
    }

    HRESULT SetColorScheme(LOGPALETTE* pLogpal) override {
        return E_NOTIMPL;
    }

private:
    ULONG m_cRef{ 1 };
    CComPtr<IOleClientSite> client_site_;
};

void BeginRun(const zaf::ApplicationBeginRunInfo& event_info);

class Window : public zaf::Window {
protected:
    void AfterParse() override {

        __super::AfterParse();

        this->RootControl()->SetBackgroundColor(zaf::Color::White());
        this->RootControl()->SetLayouter(zaf::Create<zaf::VerticalLayouter>());

        text_box_ = zaf::Create<zaf::TextBox>();
        text_box_->SetFontSize(22);
        text_box_->SetIsMultiline(true);

        this->RootControl()->AddChild(text_box_);

        InitializeOLEObject();
    }

private:
    void InitializeOLEObject() {

        auto text_service = text_box_->GetITextServices();

        CComPtr<IRichEditOle> rich_edit_ole{};
        LRESULT lresult{};
        text_service->TxSendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&rich_edit_ole, &lresult);

        CComPtr<IOleClientSite> client_site{};
        rich_edit_ole->GetClientSite(&client_site);

        CComPtr<MyOLEObject> my_object = new MyOLEObject();

        HRESULT hresult = OleSetContainedObject(static_cast<IOleObject*>(my_object), TRUE);

        REOBJECT object_info{};
        object_info.cbStruct = sizeof(object_info);
        object_info.cp = 0;
        object_info.clsid = MyOLEObjectID;
        object_info.poleobj = my_object;
        object_info.dvaspect = DVASPECT_CONTENT;
        object_info.dwFlags = REO_BELOWBASELINE;
        object_info.polesite = client_site;
        object_info.pstg = nullptr;

        auto size = zaf::FromDIPs(zaf::Size{ 100, 30 }, this->GetDPI());

        SIZEL size_in_pixels{};
        size_in_pixels.cx = (LONG)size.width;
        size_in_pixels.cy = (LONG)size.height;
        AtlPixelToHiMetric(&size_in_pixels, &object_info.sizel);

        hresult = rich_edit_ole->InsertObject(&object_info);


        int x = 0;
    }

private:
    std::shared_ptr<zaf::TextBox> text_box_;
};


int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
) {

    auto& application = zaf::Application::Instance();
    application.Subscriptions() += application.BeginRunEvent().Subscribe(BeginRun);

    application.Initialize({});
    application.Run();
}


void BeginRun(const zaf::ApplicationBeginRunInfo& event_info) {

    auto window = zaf::Create<Window>();
    window->SetIsSizable(true);
    window->SetHasTitleBar(true);
    window->SetContentSize(zaf::Size{ 600, 400 });
    window->Show();

    zaf::Application::Instance().SetMainWindow(window);
}
