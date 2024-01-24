#pragma once

#include <string_view>
#include <zaf/base/non_copyable.h>
#include <zaf/base/none.h>
#include <zaf/rx/subject.h>

namespace zaf::internal {

class TextModel : NonCopyableNonMovable {
public:
    TextModel() = default;
    virtual ~TextModel() = default;

    virtual std::wstring_view GetText() = 0;
    virtual void SetText(std::wstring_view text) = 0;

    Observable<None> TextChangedEvent() const {
        return text_changed_event_.AsObservable();
    }

protected:
    void RaiseTextChangedEvent() {
        text_changed_event_.AsObserver().OnNext({});
    }

private:
    Subject<None> text_changed_event_;
};

}