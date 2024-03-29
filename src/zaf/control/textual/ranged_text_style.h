#pragma once

#include <zaf/base/error/check.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/internal/enumerator.h>
#include <zaf/control/internal/range_map.h>
#include <zaf/control/textual/inline_object_store.h>
#include <zaf/graphic/font/font.h>

namespace zaf::textual {

template<bool IsConst>
class BaseRangedItem : NonCopyable {
private:
    using InnerItem = std::add_lvalue_reference_t<
        std::conditional_t<
            IsConst,
            std::add_const_t<internal::RangedValueStore::Item>,
            internal::RangedValueStore::Item
        >
    >;
    static_assert(std::is_reference_v<InnerItem>);

public:
    explicit BaseRangedItem(InnerItem inner) : inner_(inner) {

    }

    const Range& Range() const {
        return inner_.Range();
    }

protected:
    InnerItem inner_;
};


template<bool IsConst>
class RangedFontItem : public BaseRangedItem<IsConst> {
public:
    using BaseRangedItem<IsConst>::BaseRangedItem;

    const zaf::Font& Font() const {
        return *std::any_cast<zaf::Font>(&this->inner_.Value());
    }

    template<bool IsEnabled = !IsConst, typename K = std::enable_if_t<IsEnabled>>
    zaf::Font& Font() {
        return const_cast<zaf::Font&>(static_cast<const RangedFontItem<IsConst>*>(this)->Font());
    }
};


template<typename StoreIterator, typename Item>
class RangedItemIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Item;
    using difference_type = std::make_signed_t<std::size_t>;
    using pointer = value_type*;
    using reference = value_type&;

public:
    explicit RangedItemIterator(StoreIterator inner) : inner_(inner) {

    }

    RangedItemIterator& operator++() {
        inner_++;
        return *this;
    }

    RangedItemIterator operator++(int) {
        return RangedItemIterator{ ++inner_ };
    }

    value_type operator*() const {
        return value_type{ *inner_ };
    }

    bool operator!=(const RangedItemIterator& other) const {
        return inner_ != other.inner_;
    }

private:
    StoreIterator inner_;
};


template<bool IsConst, template<bool> typename Item>
class RangedItemEnumerator {
private:
    using StoreType = std::conditional_t<
        IsConst,
        const internal::RangedValueStore&, 
        internal::RangedValueStore&>;

    static_assert(std::is_reference_v<StoreType>);

    using StoreIterator = std::conditional_t<
        IsConst,
        typename internal::RangedValueStore::const_iterator,
        typename internal::RangedValueStore::iterator>;

    using StoreConstIterator = typename internal::RangedValueStore::const_iterator;

public:
    using value_type = Item<IsConst>;

    using iterator = RangedItemIterator<
        StoreIterator, 
        Item<std::is_const_v<std::remove_reference_t<typename StoreIterator::reference>>>>;

    using const_iterator = RangedItemIterator<
        StoreConstIterator,
        Item<std::is_const_v<std::remove_reference_t<typename StoreConstIterator::reference>>>>;

public:
    explicit RangedItemEnumerator(StoreType store) noexcept : store_(store) {

    }

    iterator begin() noexcept {
        return iterator{ store_.begin() };
    }

    iterator end() noexcept {
        return iterator{ store_.end() };
    }

    const_iterator begin() const noexcept {
        return const_iterator{ store_.begin() };
    }

    const_iterator end() const noexcept {
        return const_iterator{ store_.end() };
    }

    const_iterator cbegin() const noexcept {
        return const_iterator{ store_.begin() };
    }

    const_iterator cend() const noexcept {
        return const_iterator{ store_.end() };
    }

    bool IsEmpty() const noexcept {
        return store_.IsEmpty();
    }

private:
    StoreType store_;
};


class RangedTextStyle : NonCopyable {
public:
    using FontEnumerator = RangedItemEnumerator<false, RangedFontItem>;
    using ConstFontEnumerator = RangedItemEnumerator<true, RangedFontItem>;

    using ColorPickerMap = internal::RangeMap<zaf::ColorPicker>;

    class ColorPickerItem : NonCopyable {
    public:
        explicit ColorPickerItem(const ColorPickerMap::Item& item) : inner_(item) { }
        const Range& Range() const { return inner_.Range(); }
        const ColorPicker& ColorPicker() const { return inner_.Value(); }
    private:
        ColorPickerMap::Item inner_;
    };

    using ColorPickerEnumerator = internal::WrapEnumerator<ColorPickerMap, ColorPickerItem>;

    class InlineObjectItem : NonCopyable {
    public:
        explicit InlineObjectItem(const InlineObjectStore::ItemList::value_type& item) : 
            inner_(item) { }
        const Range& Range() const { return inner_.Range(); }
        const std::shared_ptr<InlineObject>& InlineObject() const {
            return inner_.Object();
        }
    private:
        const InlineObjectStore::ItemList::value_type& inner_;
    };

    using InlineObjectEnumerator = internal::WrapEnumerator<
        InlineObjectStore::ItemList,
        InlineObjectItem
    >;

public:
    RangedTextStyle() = default;

    ConstFontEnumerator Fonts() const {
        return ConstFontEnumerator{ fonts_ };
    }

    FontEnumerator Fonts() {
        return FontEnumerator{ fonts_ };
    }

    const Font* GetFontAtIndex(std::size_t index) const {
        auto item = fonts_.FindItemContainsIndex(index);
        if (item) {
            return std::any_cast<Font>(&item->Value());
        }
        return nullptr;
    }

    void SetFontInRange(Font font, const Range& range) {
        fonts_.AddRange(range, std::move(font));
    }

    void ClearFonts() {
        fonts_.Clear();
    }

    ColorPickerEnumerator TextColorPickers() const {
        return ColorPickerEnumerator{ text_color_pickers_ };
    }

    const ColorPicker* GetTextColorPickerAtIndex(std::size_t index) const {
        return text_color_pickers_.GetValueAtIndex(index);
    }

    void SetTextColorPickerInRange(ColorPicker color_picker, const Range& range) {
        ZAF_EXPECT(color_picker);
        text_color_pickers_.AddRange(range, std::move(color_picker));
    }

    void ClearTextColorPickers() {
        text_color_pickers_.Clear();
    }

    ColorPickerEnumerator TextBackColorPickers() const {
        return ColorPickerEnumerator{ text_back_color_pickers_ };
    }

    const ColorPicker* GetTextBackColorPickerAtIndex(std::size_t index) const {
        return text_back_color_pickers_.GetValueAtIndex(index);
    }

    void SetTextBackColorPickerInRange(ColorPicker color_picker, const Range& range) {
        ZAF_EXPECT(color_picker);
        text_back_color_pickers_.AddRange(range, std::move(color_picker));
    }

    void ClearTextBackColorPickers() {
        text_back_color_pickers_.Clear();
    }

    InlineObjectEnumerator InlineObjects() const {
        return InlineObjectEnumerator{ inline_objects_.Items() };
    }

    std::shared_ptr<InlineObject> GetInlineObjectAtIndex(std::size_t index) const {
        return inline_objects_.GetInlineObjectAtIndex(index);
    }

    void AttachInlineObjectToRange(std::shared_ptr<InlineObject> object, const Range& range) {
        inline_objects_.Attach(std::move(object), range);
    }

    void ClearInlineObjects() {
        inline_objects_.Clear();
    }

    void ReplaceSpan(
        const Range& span_range, 
        std::size_t new_length) {

        fonts_.ReplaceSpan(span_range, new_length);
        text_color_pickers_.ReplaceSpan(span_range, new_length);
        text_back_color_pickers_.ReplaceSpan(span_range, new_length);
        inline_objects_.ReplaceSpan(span_range, new_length);
    }

    void Clear() {
        fonts_.Clear();
        text_color_pickers_.Clear();
        text_back_color_pickers_.Clear();
        inline_objects_.Clear();
    }

    RangedTextStyle Clone() const;

private:
    internal::RangedValueStore fonts_;
    ColorPickerMap text_color_pickers_;
    ColorPickerMap text_back_color_pickers_;
    InlineObjectStore inline_objects_;
};

}