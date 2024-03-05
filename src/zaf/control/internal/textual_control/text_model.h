#pragma once

#include <string_view>
#include <zaf/base/flag_enum.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/none.h>
#include <zaf/base/range.h>
#include <zaf/control/internal/textual_control/styled_text.h>
#include <zaf/rx/subject.h>

namespace zaf::internal {

enum class TextModelAttribute : std::uint32_t {
    None = 0,
    Text = 1 << 1,
    Font = 1 << 2,
    TextColor = 1 << 3,
    InlineObject = 1 << 4,
    All = ~None,
};

ZAF_ENABLE_FLAG_ENUM(TextModelAttribute);


class TextModelChangedInfo {
public:
    class ChangedRangeInfo {
    public:
        ChangedRangeInfo(const zaf::Range& range, std::size_t new_length) :
            range_(range),
            new_length_(new_length) {

        }

        const zaf::Range& Range() const {
            return range_;
        }

        std::size_t NewLength() const {
            return new_length_;
        }

    private:
        zaf::Range range_;
        std::size_t new_length_;
    };

public:
    explicit TextModelChangedInfo(TextModelAttribute changed_attributes) :
        changed_attributes_(changed_attributes) {

    }

    TextModelChangedInfo(
        TextModelAttribute changed_attributes,
        const Range& changed_range, 
        std::size_t new_length) 
        :
        changed_attributes_(changed_attributes),
        changed_range_info_(ChangedRangeInfo{ changed_range, new_length }) {

    }

    TextModelAttribute ChangedAttributes() const {
        return changed_attributes_;
    }

    const std::optional<ChangedRangeInfo>& ChangedRange() const {
        return changed_range_info_;
    }

    bool IsTextChanged() const {
        return HasFlag(changed_attributes_, TextModelAttribute::Text);
    }

    bool IsFontChanged() const {
        return HasFlag(changed_attributes_, TextModelAttribute::Font);
    }

    bool IsTextColorChanged() const {
        return HasFlag(changed_attributes_, TextModelAttribute::TextColor);
    }

    bool IsInlineObjectChanged() const {
        return HasFlag(changed_attributes_, TextModelAttribute::InlineObject);
    }

private:
    std::optional<ChangedRangeInfo> changed_range_info_;
    TextModelAttribute changed_attributes_{ TextModelAttribute::None };
};


class TextModel : NonCopyableNonMovable {
public:
    TextModel() = default;
    ~TextModel() = default;

    const StyledText& StyledText() const {
        return styled_text_;
    }

    const std::wstring& GetText() const {
        return styled_text_.Text();
    }

    void SetText(std::wstring text);
    void SetTextInRange(std::wstring_view text, const Range& range);

    void SetFont(Font font);
    void SetFontInRange(Font font, const Range& range);

    void SetTextColorPicker(ColorPicker picker);
    void SetTextColorPickerInRange(ColorPicker picker, const Range& range);

    void AttachInlineObjectToRange(
        std::shared_ptr<textual::InlineObject> object,
        const Range& range);

    void ReplaceStyledTextSlice(const Range& replaced_range, const StyledTextSlice& slice);

    Observable<TextModelChangedInfo> TextChangedEvent() const {
        return changed_event_.AsObservable();
    }

private:
    void RaiseChangedEvent(TextModelAttribute attributes) {
        changed_event_.AsObserver().OnNext(TextModelChangedInfo{ attributes });
    }

    void RaiseChangedEvent(
        TextModelAttribute attributes,
        const Range& changed_range,
        std::size_t new_length) {

        changed_event_.AsObserver().OnNext(TextModelChangedInfo{ 
            attributes,
            changed_range,
            new_length
        });
    }

private:
    internal::StyledText styled_text_;
    Subject<TextModelChangedInfo> changed_event_;
};

}