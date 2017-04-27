#include <zaf/graphic/text/text_format.h>

namespace zaf {

TextTrimming TextFormat::GetTextTrimming(std::error_code& error_code) const {

    DWRITE_TRIMMING dwrite_trimming = { };
    IDWriteInlineObject* dwrite_inline_object = nullptr;
    HRESULT hresult = GetHandle()->GetTrimming(&dwrite_trimming, &dwrite_inline_object);

    error_code = MakeComErrorCode(hresult);

    TextTrimming text_trimming;

    if (IsSucceeded(error_code)) {
        text_trimming.granularity = static_cast<TextTrimming::Granularity>(dwrite_trimming.granularity);
        text_trimming.delimiter = dwrite_trimming.delimiter;
        text_trimming.delimiter_count = dwrite_trimming.delimiterCount;
        text_trimming.trimming_sign = dwrite_inline_object;
    }
    
    return text_trimming;
}


void TextFormat::SetTextTrimming(const TextTrimming& text_trimming, std::error_code& error_code) {

    DWRITE_TRIMMING dwrite_trimming = { };
    dwrite_trimming.granularity = static_cast<DWRITE_TRIMMING_GRANULARITY>(text_trimming.granularity);
    dwrite_trimming.delimiter = text_trimming.delimiter;
    dwrite_trimming.delimiterCount = text_trimming.delimiter_count;
    HRESULT hresult = GetHandle()->SetTrimming(&dwrite_trimming, text_trimming.trimming_sign.GetHandle());
    
    error_code = MakeComErrorCode(hresult);
}

}