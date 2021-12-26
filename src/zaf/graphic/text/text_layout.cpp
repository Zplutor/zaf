#include <zaf/graphic/text/text_layout.h>
#include <memory>
#include <zaf/base/error/com_error.h>

namespace zaf {

std::wstring TextLayout::GetFontFamilyName(std::size_t position, TextRange* range) const {

    UINT32 family_name_length = 0;
    HRESULT result = GetHandle()->GetFontFamilyNameLength(
        static_cast<UINT32>(position),
        &family_name_length);

    ZAF_THROW_IF_COM_ERROR(result);

    if (family_name_length == 0) {
        return std::wstring();
    }

    const std::size_t buffer_length = family_name_length + 1;
    auto buffer = std::make_unique<wchar_t[]>(buffer_length);
    DWRITE_TEXT_RANGE text_range = { 0 };
    result = GetHandle()->GetFontFamilyName(
        static_cast<UINT32>(position),
        buffer.get(), 
        static_cast<UINT32>(buffer_length),
        range == nullptr ? nullptr : &text_range);

    ZAF_THROW_IF_COM_ERROR(result);

    if (range != nullptr) {
        *range = TextRange::FromDWRITETEXTRANGE(text_range);
    }

    return buffer.get();
}


float TextLayout::GetFontSize(std::size_t position, TextRange* range) const {

    float font_size = 0;
    DWRITE_TEXT_RANGE text_range = { 0 };
    HRESULT result = GetHandle()->GetFontSize(
        static_cast<UINT32>(position),
        &font_size, 
        range == nullptr ? nullptr : &text_range);

    ZAF_THROW_IF_COM_ERROR(result);
    
    if (range != nullptr) {
        *range = TextRange::FromDWRITETEXTRANGE(text_range);
    }

    return font_size;
}


FontStyle TextLayout::GetFontStyle(std::size_t position, TextRange* range) const {

    DWRITE_FONT_STYLE font_style = DWRITE_FONT_STYLE_NORMAL;
    DWRITE_TEXT_RANGE text_range = { 0 };
    HRESULT result = GetHandle()->GetFontStyle(
        static_cast<UINT32>(position),
        &font_style, 
        range == nullptr ? nullptr : &text_range);

    ZAF_THROW_IF_COM_ERROR(result);

    if (range != nullptr) {
        *range = TextRange::FromDWRITETEXTRANGE(text_range);
    }

    return static_cast<FontStyle>(font_style);
}


int TextLayout::GetFontWeight(std::size_t position, TextRange* range) const {

    DWRITE_FONT_WEIGHT font_weight = static_cast<DWRITE_FONT_WEIGHT>(0);
    DWRITE_TEXT_RANGE text_range = { 0 };
    HRESULT result = GetHandle()->GetFontWeight(
        static_cast<UINT32>(position),
        &font_weight, 
        range == nullptr ? nullptr : &text_range);

    ZAF_THROW_IF_COM_ERROR(result);

    if (range != nullptr) {
        *range = TextRange::FromDWRITETEXTRANGE(text_range);
    }

    return font_weight;
}


bool TextLayout::HasUnderline(std::size_t position, TextRange* range) const {

    BOOL has_underline = FALSE;
    DWRITE_TEXT_RANGE text_range = { 0 };
    HRESULT result = GetHandle()->GetUnderline(
        static_cast<UINT32>(position),
        &has_underline, 
        range == nullptr ? nullptr : &text_range);

    ZAF_THROW_IF_COM_ERROR(result);

    if (range != nullptr) {
        *range = TextRange::FromDWRITETEXTRANGE(text_range);
    }

    return has_underline ? TRUE : FALSE;
}


Brush TextLayout::GetBrush(std::size_t position, TextRange* range) {

    IUnknown* drawing_effect = nullptr;
    DWRITE_TEXT_RANGE text_range = { 0 };
    HRESULT result = GetHandle()->GetDrawingEffect(
        static_cast<UINT32>(position),
        &drawing_effect, 
        &text_range);

    ZAF_THROW_IF_COM_ERROR(result);

    ID2D1Brush* brush_handle = nullptr;
    result = drawing_effect->QueryInterface(&brush_handle);
    drawing_effect->Release();
    ZAF_THROW_IF_COM_ERROR(result);

    return Brush(brush_handle);
}


std::vector<LineMetrics> TextLayout::GetLineMetrics(std::size_t max_line_count) const {

    std::vector<LineMetrics> line_metrics;
    line_metrics.resize(max_line_count);

    UINT32 actual_line_count = 0;
    HRESULT result = GetHandle()->GetLineMetrics(
        reinterpret_cast<DWRITE_LINE_METRICS*>(line_metrics.data()),
        static_cast<UINT32>(line_metrics.size()),
        &actual_line_count);

    ZAF_THROW_IF_COM_ERROR(result);

    return line_metrics;
}


TextMetrics TextLayout::GetMetrics() const {

    TextMetrics text_metrics;
    HRESULT result = GetHandle()->GetMetrics(&text_metrics.Inner());

    ZAF_THROW_IF_COM_ERROR(result);

    return text_metrics;
}

}