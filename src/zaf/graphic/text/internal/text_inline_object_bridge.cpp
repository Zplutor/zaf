#include <zaf/graphic/text/internal/text_inline_object_bridge.h>
#include <zaf/base/error/contract_error.h>

namespace zaf::internal {

TextInlineObjectBridge::TextInlineObjectBridge(
    std::shared_ptr<textual::InlineObject> custom_object,
    std::shared_ptr<TextInlineObjectPainter> painter)
    :
    custom_object_(std::move(custom_object)),
    painter_(std::move(painter)) {

    ZAF_EXPECT(custom_object_);
    ZAF_EXPECT(painter_);
}


HRESULT __stdcall TextInlineObjectBridge::QueryInterface(REFIID riid, void** ppvObject) {

    if (!ppvObject) {
        return E_INVALIDARG;
    }

    if (riid == IID_IUnknown || 
        riid == __uuidof(IDWriteInlineObject) || 
        riid == IID_TextInlineObjectBridge) {

        *ppvObject = this;
        AddRef();
        return S_OK;
    }

    *ppvObject = nullptr;
    return E_NOINTERFACE;
}


ULONG __stdcall TextInlineObjectBridge::AddRef(void) {
    return InterlockedIncrement(&reference_count_);
}


ULONG __stdcall TextInlineObjectBridge::Release(void) {
    ULONG result = InterlockedDecrement(&reference_count_);
    if (result == 0) {
        delete this;
    }
    return result;
}


HRESULT __stdcall TextInlineObjectBridge::Draw(
    void* clientDrawingContext,
    IDWriteTextRenderer* renderer,
    FLOAT originX, 
    FLOAT originY, 
    BOOL isSideways,
    BOOL isRightToLeft, 
    IUnknown* clientDrawingEffect) {

    auto metrics = custom_object_->GetMetrics();

    Rect object_rect{ originX, originY, metrics.Width(), metrics.Height() };
    auto canvas = painter_->Canvas();
    auto guard = canvas->PushRegion(object_rect, object_rect);
    custom_object_->Paint(*canvas);
    return S_OK;
}


HRESULT __stdcall TextInlineObjectBridge::GetMetrics(DWRITE_INLINE_OBJECT_METRICS* metrics) {

    *metrics = custom_object_->GetMetrics().Inner();
    return S_OK;
}


HRESULT __stdcall TextInlineObjectBridge::GetOverhangMetrics(DWRITE_OVERHANG_METRICS* overhangs) {
    return E_NOTIMPL;
}


HRESULT __stdcall TextInlineObjectBridge::GetBreakConditions(
    DWRITE_BREAK_CONDITION* breakConditionBefore,
    DWRITE_BREAK_CONDITION* breakConditionAfter) {

    return E_NOTIMPL;
}

}