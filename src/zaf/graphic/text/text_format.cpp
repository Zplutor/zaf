#include <zaf/graphic/text/text_format.h>
#include <zaf/base/error/com_error.h>

namespace zaf {

TextTrimming TextFormat::GetTextTrimming() const {

    DWRITE_TRIMMING dwrite_trimming = { };
    IDWriteInlineObject* dwrite_inline_object = nullptr;
    HRESULT hresult = Inner()->GetTrimming(&dwrite_trimming, &dwrite_inline_object);

    ZAF_THROW_IF_COM_ERROR(hresult);

    TextTrimming text_trimming;
    text_trimming.granularity = static_cast<TextTrimming::Granularity>(dwrite_trimming.granularity);
    text_trimming.delimiter = dwrite_trimming.delimiter;
    text_trimming.delimiter_count = dwrite_trimming.delimiterCount;
    text_trimming.trimming_sign = dwrite_inline_object;
    return text_trimming;
}


void TextFormat::SetTextTrimming(const TextTrimming& text_trimming) {

    DWRITE_TRIMMING dwrite_trimming = { };
    dwrite_trimming.granularity = static_cast<DWRITE_TRIMMING_GRANULARITY>(text_trimming.granularity);
    dwrite_trimming.delimiter = text_trimming.delimiter;
    dwrite_trimming.delimiterCount = text_trimming.delimiter_count;
    HRESULT hresult = Inner()->SetTrimming(&dwrite_trimming, text_trimming.trimming_sign.Inner());
    
    ZAF_THROW_IF_COM_ERROR(hresult);
}

}