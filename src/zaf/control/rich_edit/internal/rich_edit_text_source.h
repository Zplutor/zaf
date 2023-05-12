#pragma once

#include <Richedit.h>
#include <TextServ.h>
#include <zaf/base/com_object.h>
#include <zaf/control/text_source.h>

namespace zaf::rich_edit::internal {

class RichEditTextSource : public TextSource {
public:
    explicit RichEditTextSource(const COMObject<ITextServices2>& text_service);

    std::variant<std::wstring_view, std::wstring> GetText() override;
    TextSourceChangeResult SetText(const std::wstring& text) override;

private:
    COMObject<ITextServices2> text_service_;
};

}