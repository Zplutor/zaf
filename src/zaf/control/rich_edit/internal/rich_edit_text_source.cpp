#include <zaf/control/rich_edit/internal/rich_edit_text_source.h>
#include <zaf/base/error/check.h>

namespace zaf::rich_edit::internal {

RichEditTextSource::RichEditTextSource(ITextServices2* text_service) : 
    text_service_(text_service)  {

    ZAF_EXPECT(text_service_);
}


std::variant<std::wstring_view, std::wstring> RichEditTextSource::GetText() {

    std::wstring text;
    BSTR text_buffer{};
    HRESULT result = text_service_->TxGetText(&text_buffer);
    if (SUCCEEDED(result) && text_buffer) {

        text.assign(text_buffer);
        SysFreeString(text_buffer);
    }
    return text;
}


TextSourceChangeResult RichEditTextSource::SetText(const std::wstring& text) {

    text_service_->TxSetText(text.c_str());

    TextSourceChangeResult result;
    result.SetIsChanged(true);
    //Rich edit will send text changed notification itself.
    result.SetIsNotificationSent(true);
    return result;
}


}