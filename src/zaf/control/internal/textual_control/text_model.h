#pragma once

#include <string_view>
#include <zaf/base/non_copyable.h>
#include <zaf/base/none.h>
#include <zaf/base/range.h>
#include <zaf/rx/subject.h>

namespace zaf::internal {

class TextModel : NonCopyableNonMovable {
public:
    TextModel() = default;
    ~TextModel() = default;

    std::wstring_view GetText() const {
        return text_;
    }

    void SetText(std::wstring_view text);
    void SetTextInRange(std::wstring_view text, const Range& range);

    Observable<None> TextChangedEvent() const {
        return text_changed_event_.AsObservable();
    }

protected:
    void RaiseTextChangedEvent() {
        text_changed_event_.AsObserver().OnNext({});
    }

private:
    std::wstring text_;
    Subject<None> text_changed_event_;
};

}