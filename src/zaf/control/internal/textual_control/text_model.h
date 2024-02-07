#pragma once

#include <string_view>
#include <zaf/base/non_copyable.h>
#include <zaf/base/none.h>
#include <zaf/base/range.h>
#include <zaf/rx/subject.h>

namespace zaf::internal {

class TextModelChangedInfo {
public:
    TextModelChangedInfo(const Range& replaced_range, const Range& new_range) :
        replaced_range_(replaced_range),
        new_range_(new_range) {

    }

    const Range& ReplacedRange() const {
        return replaced_range_;
    }

    const Range& NewRange() const {
        return new_range_;
    }

private:
    Range replaced_range_;
    Range new_range_;
};

class TextModel : NonCopyableNonMovable {
public:
    TextModel() = default;
    ~TextModel() = default;

    std::wstring_view GetText() const {
        return text_;
    }

    void SetText(std::wstring_view text);
    void SetTextInRange(std::wstring_view text, const Range& range);

    Observable<TextModelChangedInfo> TextChangedEvent() const {
        return text_changed_event_.AsObservable();
    }

protected:
    void RaiseTextChangedEvent(const Range& replaced_range, const Range& new_range) {
        text_changed_event_.AsObserver().OnNext(TextModelChangedInfo{ replaced_range, new_range });
    }

private:
    std::wstring text_;
    Subject<TextModelChangedInfo> text_changed_event_;
};

}