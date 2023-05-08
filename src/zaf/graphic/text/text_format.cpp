#include <zaf/graphic/text/text_format.h>
#include <zaf/base/error/com_error.h>

namespace zaf {

TextTrimming TextFormat::GetTextTrimming() const {

    TextTrimming result;
    HRESULT hresult = Inner()->GetTrimming(&result.Inner(), result.Sign().Reset());

    ZAF_THROW_IF_COM_ERROR(hresult);
    return result;
}


void TextFormat::SetTextTrimming(const TextTrimming& trimming) {

    HRESULT hresult = Inner()->SetTrimming(&trimming.Inner(), trimming.Sign().Inner());
    ZAF_THROW_IF_COM_ERROR(hresult);
}

}