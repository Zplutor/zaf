#pragma once

#include <string_view>
#include <zaf/base/flags_enum.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/none.h>
#include <zaf/base/range.h>
#include <zaf/control/style/color_picker.h>
#include <zaf/control/textual/styled_text.h>
#include <zaf/internal/control/color_picker_equal.h>
#include <zaf/internal/textual/inline_object_attached_info.h>
#include <zaf/rx/subject.h>

namespace zaf::internal {

enum class TextModelAttribute : std::uint32_t {
    None = 0,
    Text = 1 << 1,
    Font = 1 << 2,
    TextColor = 1 << 3,
    TextBackColor = 1 << 4,
    InlineObject = 1 << 5,
    All = ~None,
};

ZAF_ENABLE_FLAGS_ENUM(TextModelAttribute);


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

    bool IsTextBackColorChanged() const {
        return HasFlag(changed_attributes_, TextModelAttribute::TextBackColor);
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
    TextModel();
    ~TextModel() = default;

    bool IsMultiline() const noexcept;
    void SetIsMultiline(bool is_multiline);

    const textual::StyledText& StyledText() const {
        return styled_text_;
    }

    void SetStyledText(textual::StyledText styled_text);

    const std::wstring& GetText() const {
        return styled_text_.Text();
    }

    void SetText(std::wstring text);
    void SetTextInRange(std::wstring_view text, const Range& range);

    void SetFont(Font font);
    void SetFontInRange(Font font, const Range& range);

    void SetTextColor(const Color& color);
    void SetTextColorPicker(ColorPicker picker, const Control& owner);

    void SetTextColorInRange(const Color& color, const Range& range);
    void SetTextColorPickerInRange(ColorPicker picker, const Range& range, const Control& owner);

    void SetTextBackColor(const Color& color);
    void SetTextBackColorInRange(const Color& color, const Range& range);

    void SetTextBackColorPicker(ColorPicker picker, const Control& owner);
    void SetTextBackColorPickerInRange(ColorPicker picker, const Range& range, const Control& owner);

    void UpdateColors(const Control& owner);

    const ColorPicker& DefaultTextColorPicker() const {
        return default_text_color_picker_;
    }

    const ColorPicker& DefaultTextBackColorPicker() const {
        return default_text_back_color_picker_;
    }

    void AttachInlineObjectToRange(
        std::shared_ptr<textual::InlineObject> object,
        const Range& range);

    void ReplaceStyledTextSlice(
        const Range& replaced_range,
        const textual::StyledText& slice);

    Observable<InlineObjectAttachedInfo> InlineObjectAttachedEvent() const {
        return inline_object_attached_event_.AsObservable();
    }

    Observable<TextModelChangedInfo> TextChangedEvent() const {
        return changed_event_.AsObservable();
    }

private:
    void InnerSetTextColor(const Color& color);
    void InnerSetTextColorInRange(const Color& color, const Range& range);

    void InnerSetTextBackColor(const Color& color);
    void InnerSetTextBackColorInRange(const Color& color, const Range& range);

    void RaiseInlineObjectAttachedEvent(
        std::vector<std::shared_ptr<textual::InlineObject>> objects) {

        if (objects.empty()) {
            return;
        }

        inline_object_attached_event_.AsObserver().OnNext(InlineObjectAttachedInfo{
            std::move(objects),
        });
    }

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
    bool is_multiline_{ true };
    textual::StyledText styled_text_;

    ColorPicker default_text_color_picker_;
    RangeMap<ColorPicker, internal::ColorPickerEqual> ranged_text_color_pickers_;

    ColorPicker default_text_back_color_picker_;
    RangeMap<ColorPicker, internal::ColorPickerEqual> ranged_text_back_color_pickers_;

    Subject<InlineObjectAttachedInfo> inline_object_attached_event_;
    Subject<TextModelChangedInfo> changed_event_;
};

}