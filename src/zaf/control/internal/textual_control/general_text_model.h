#pragma once

#include <zaf/control/internal/textual_control/text_model.h>

namespace zaf::internal {

class GeneralTextModel : public TextModel {
public:
    std::wstring_view GetText() override {
        return text_;
    }

    void SetText(std::wstring_view text) override {
        text_ = text;
        RaiseTextChangedEvent();
    }

private:
    std::wstring text_;
};

}