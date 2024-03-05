#pragma once

#include <dwrite.h>
#include <zaf/control/textual/inline_object.h>
#include <zaf/graphic/text/internal/text_inline_object_painter.h>

namespace zaf::internal {

// {A7A58B39-AF3F-4746-B794-88700EEE41BF}
constexpr GUID IID_TextInlineObjectBridge = {
    0xa7a58b39, 0xaf3f, 0x4746, { 0xb7, 0x94, 0x88, 0x70, 0xe, 0xee, 0x41, 0xbf } 
};


class TextInlineObjectBridge : public IDWriteInlineObject {
public:
    TextInlineObjectBridge(
        std::shared_ptr<textual::InlineObject> custom_object,
        std::shared_ptr<TextInlineObjectPainter> painter);

    HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override;
    ULONG __stdcall AddRef(void) override;
    ULONG __stdcall Release(void) override;

    HRESULT __stdcall Draw(
        void* clientDrawingContext, 
        IDWriteTextRenderer* renderer,
        FLOAT originX, 
        FLOAT originY,
        BOOL isSideways, 
        BOOL isRightToLeft,
        IUnknown* clientDrawingEffect) override;

    HRESULT __stdcall GetMetrics(DWRITE_INLINE_OBJECT_METRICS* metrics) override;
    HRESULT __stdcall GetOverhangMetrics(DWRITE_OVERHANG_METRICS* overhangs) override;

    HRESULT __stdcall GetBreakConditions(
        DWRITE_BREAK_CONDITION* breakConditionBefore,
        DWRITE_BREAK_CONDITION* breakConditionAfter) override;

private:
    LONG reference_count_{ 1 };
    std::shared_ptr<textual::InlineObject> custom_object_;
    std::shared_ptr<TextInlineObjectPainter> painter_;
};

}