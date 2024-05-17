#include <zaf/graphic/text/text_format.h>
#include <zaf/base/error/com_error.h>

namespace zaf {

TextTrimming TextFormat::GetTextTrimming() const {

    TextTrimming result;
    COMPtr<IDWriteInlineObject> trimming_sign;
    HRESULT hresult = Ptr()->GetTrimming(&result.Inner(), trimming_sign.Reset());
    ZAF_THROW_IF_COM_ERROR(hresult);

    result.SetSign(TextInlineObject{ trimming_sign });
    return result;
}


void TextFormat::SetTextTrimming(const TextTrimming& trimming) {

    HRESULT hresult = Ptr()->SetTrimming(
        &trimming.Inner(), 
        trimming.Sign().Ptr().Inner());

    ZAF_THROW_IF_COM_ERROR(hresult);
}


LineSpacing TextFormat::GetLineSpacing() const {

    DWRITE_LINE_SPACING_METHOD method{};
    float line_height{};
    float baseline{};

    HRESULT hresult = Ptr()->GetLineSpacing(&method, &line_height, &baseline);
    ZAF_THROW_IF_COM_ERROR(hresult);
    
    return LineSpacing{ static_cast<LineSpacingMethod>(method), line_height, baseline };
}


void TextFormat::SetLineSpacing(const LineSpacing& line_spacing) {

    HRESULT hresult = Ptr()->SetLineSpacing(
        static_cast<DWRITE_LINE_SPACING_METHOD>(line_spacing.Method()),
        line_spacing.LineHeight(),
        line_spacing.HeightAboveBaseline());

    ZAF_THROW_IF_COM_ERROR(hresult);
}

}