#include <zaf/graphic/text/text_format.h>
#include <zaf/base/error/com_error.h>

namespace zaf {

TextTrimming TextFormat::GetTextTrimming() const {

    TextTrimming result;
    COMPtr<IDWriteInlineObject> trimming_sign;
    HRESULT hresult = Inner()->GetTrimming(&result.Inner(), trimming_sign.Reset());
    ZAF_THROW_IF_COM_ERROR(hresult);

    result.SetSign(TextInlineObject{ trimming_sign });
    return result;
}


void TextFormat::SetTextTrimming(const TextTrimming& trimming) {

    HRESULT hresult = Inner()->SetTrimming(
        &trimming.Inner(), 
        trimming.Sign().Inner().Inner());

    ZAF_THROW_IF_COM_ERROR(hresult);
}

}