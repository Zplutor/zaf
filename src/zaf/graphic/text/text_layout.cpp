#include <zaf/graphic/text/text_layout.h>
#include <memory>
#include <zaf/base/error/com_error.h>

namespace zaf {

std::wstring TextLayout::GetFontFamilyName(std::size_t position, Range* range) const {

    UINT32 family_name_length = 0;
    HRESULT result = Inner()->GetFontFamilyNameLength(
        static_cast<UINT32>(position),
        &family_name_length);

    ZAF_THROW_IF_COM_ERROR(result);

    if (family_name_length == 0) {
        return std::wstring();
    }

    const std::size_t buffer_length = family_name_length + 1;
    auto buffer = std::make_unique<wchar_t[]>(buffer_length);
    DWRITE_TEXT_RANGE text_range = { 0 };
    result = Inner()->GetFontFamilyName(
        static_cast<UINT32>(position),
        buffer.get(), 
        static_cast<UINT32>(buffer_length),
        range == nullptr ? nullptr : &text_range);

    ZAF_THROW_IF_COM_ERROR(result);

    if (range != nullptr) {
        *range = Range::FromDWRITETEXTRANGE(text_range);
    }

    return buffer.get();
}


float TextLayout::GetFontSize(std::size_t position, Range* range) const {

    float font_size = 0;
    DWRITE_TEXT_RANGE text_range = { 0 };
    HRESULT result = Inner()->GetFontSize(
        static_cast<UINT32>(position),
        &font_size, 
        range == nullptr ? nullptr : &text_range);

    ZAF_THROW_IF_COM_ERROR(result);
    
    if (range != nullptr) {
        *range = Range::FromDWRITETEXTRANGE(text_range);
    }

    return font_size;
}


FontStyle TextLayout::GetFontStyle(std::size_t position, Range* range) const {

    DWRITE_FONT_STYLE font_style = DWRITE_FONT_STYLE_NORMAL;
    DWRITE_TEXT_RANGE text_range = { 0 };
    HRESULT result = Inner()->GetFontStyle(
        static_cast<UINT32>(position),
        &font_style, 
        range == nullptr ? nullptr : &text_range);

    ZAF_THROW_IF_COM_ERROR(result);

    if (range != nullptr) {
        *range = Range::FromDWRITETEXTRANGE(text_range);
    }

    return static_cast<FontStyle>(font_style);
}


int TextLayout::GetFontWeight(std::size_t position, Range* range) const {

    DWRITE_FONT_WEIGHT font_weight = static_cast<DWRITE_FONT_WEIGHT>(0);
    DWRITE_TEXT_RANGE text_range = { 0 };
    HRESULT result = Inner()->GetFontWeight(
        static_cast<UINT32>(position),
        &font_weight, 
        range == nullptr ? nullptr : &text_range);

    ZAF_THROW_IF_COM_ERROR(result);

    if (range != nullptr) {
        *range = Range::FromDWRITETEXTRANGE(text_range);
    }

    return font_weight;
}


bool TextLayout::HasUnderline(std::size_t position, Range* range) const {

    BOOL has_underline = FALSE;
    DWRITE_TEXT_RANGE text_range = { 0 };
    HRESULT result = Inner()->GetUnderline(
        static_cast<UINT32>(position),
        &has_underline, 
        range == nullptr ? nullptr : &text_range);

    ZAF_THROW_IF_COM_ERROR(result);

    if (range != nullptr) {
        *range = Range::FromDWRITETEXTRANGE(text_range);
    }

    return has_underline ? TRUE : FALSE;
}


Brush TextLayout::GetBrush(std::size_t position, Range* range) {

    IUnknown* drawing_effect = nullptr;
    DWRITE_TEXT_RANGE text_range = { 0 };
    HRESULT result = Inner()->GetDrawingEffect(
        static_cast<UINT32>(position),
        &drawing_effect, 
        &text_range);

    ZAF_THROW_IF_COM_ERROR(result);

    COMPtr<ID2D1Brush> brush_inner;
    result = drawing_effect->QueryInterface(brush_inner.Reset());
    drawing_effect->Release();
    ZAF_THROW_IF_COM_ERROR(result);

    return Brush(brush_inner);
}


std::vector<LineMetrics> TextLayout::GetLineMetrics() const {

    UINT32 line_count{};
    HRESULT hresult = Inner()->GetLineMetrics(nullptr, 0, &line_count);
    if (hresult != E_NOT_SUFFICIENT_BUFFER) {
        ZAF_THROW_IF_COM_ERROR(hresult);
    }

    std::vector<LineMetrics> result;
    result.resize(line_count);

    hresult = Inner()->GetLineMetrics(
        reinterpret_cast<DWRITE_LINE_METRICS*>(result.data()),
        static_cast<UINT32>(result.size()),
        &line_count);

    ZAF_THROW_IF_COM_ERROR(hresult);

    result.resize(line_count);
    return result;
}


TextMetrics TextLayout::GetMetrics() const {

    TextMetrics text_metrics;
    HRESULT result = Inner()->GetMetrics(&text_metrics.Inner());

    ZAF_THROW_IF_COM_ERROR(result);

    return text_metrics;
}


HitTestIndexResult TextLayout::HitTestIndex(std::size_t index, bool is_trailing_hit) const {

    HitTestIndexResult result;
    HRESULT hresult = Inner()->HitTestTextPosition(
        static_cast<UINT32>(index),
        is_trailing_hit,
        &result.Inner().x,
        &result.Inner().y,
        reinterpret_cast<DWRITE_HIT_TEST_METRICS*>(&result.Inner().metrics));

    ZAF_THROW_IF_COM_ERROR(hresult);
    return result;
}


HitTestPointResult TextLayout::HitTestPoint(const Point& point) const {

    HitTestPointResult result;
    HRESULT hresult = Inner()->HitTestPoint(
        point.x,
        point.y,
        &result.Inner().is_trailing_hit,
        &result.Inner().is_inside,
        reinterpret_cast<DWRITE_HIT_TEST_METRICS*>(&result.Inner().metrics));

    ZAF_THROW_IF_COM_ERROR(hresult);
    return result;
}

}