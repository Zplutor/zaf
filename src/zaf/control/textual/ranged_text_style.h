#pragma once

#include <optional>
#include <zaf/base/error/contract.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/textual/inline_object_store.h>
#include <zaf/graphic/font/font.h>
#include <zaf/internal/ranged_value_store.h>

namespace zaf::textual {

template<bool IsConst>
class BaseRangedItem : NonCopyableNonMovable {
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

    template<bool IsEnabled = !IsConst, typename E = std::enable_if_t<IsEnabled>>
    zaf::Font& Font() {
        return const_cast<zaf::Font&>(static_cast<const RangedFontItem<IsConst>*>(this)->Font());
    }
};


template<bool IsConst>
class RangedColorPickerItem : public BaseRangedItem<IsConst> {
public:
    using BaseRangedItem<IsConst>::BaseRangedItem;

    const zaf::ColorPicker& ColorPicker() const {
        return *std::any_cast<zaf::ColorPicker>(&this->inner_.Value());
    }

    template<bool IsEnabled = !IsConst, typename E = std::enable_if_t<IsEnabled>>
    const zaf::ColorPicker& ColorPicker() {
        return const_cast<zaf::ColorPicker&>(
            static_cast<const RangedColorPickerItem<IsConst>*>(this)->ColorPicker());
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

    pointer operator->() const {
        temp_item_.emplace(*inner_);
        return &*temp_item_;
    }

    reference operator*() const {
        temp_item_.emplace(*inner_);
        return *temp_item_;
    }

    bool operator!=(const RangedItemIterator& other) const {
        return inner_ != other.inner_;
    }

private:
    StoreIterator inner_;
    mutable std::optional<Item> temp_item_;
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

    using ColorPickerEnumerator = RangedItemEnumerator<false, RangedColorPickerItem>;
    using ConstColorPickerEnumerator = RangedItemEnumerator<true, RangedColorPickerItem>;

    using InlineObjectEnumerator = InlineObjectStore::ItemList;

public:
    RangedTextStyle() = default;

    ConstFontEnumerator Fonts() const {
        return ConstFontEnumerator{ fonts_ };
    }

    FontEnumerator Fonts() {
        return FontEnumerator{ fonts_ };
    }

    const Font* GetFontAtIndex(std::size_t index) const;

    void SetFontInRange(Font font, const Range& range) {
        fonts_.AddRange(range, std::move(font));
    }

    void ClearFonts() {
        fonts_.Clear();
    }

    ConstColorPickerEnumerator TextColorPickers() const {
        return ConstColorPickerEnumerator{ text_color_pickers_ };
    }

    ColorPickerEnumerator TextColorPickers() {
        return ColorPickerEnumerator{ text_color_pickers_ };
    }

    const ColorPicker* GetTextColorPickerAtIndex(std::size_t index) const;

    void SetTextColorPickerInRange(ColorPicker color_picker, const Range& range) {
        ZAF_EXPECT(color_picker);
        text_color_pickers_.AddRange(range, std::move(color_picker));
    }

    void ClearTextColorPickers() {
        text_color_pickers_.Clear();
    }

    ConstColorPickerEnumerator TextBackColorPickers() const {
        return ConstColorPickerEnumerator{ text_back_color_pickers_ };
    }

    ColorPickerEnumerator TextBackColorPickers() {
        return ColorPickerEnumerator{ text_back_color_pickers_ };
    }

    const ColorPicker* GetTextBackColorPickerAtIndex(std::size_t index) const;

    void SetTextBackColorPickerInRange(ColorPicker color_picker, const Range& range) {
        ZAF_EXPECT(color_picker);
        text_back_color_pickers_.AddRange(range, std::move(color_picker));
    }

    void ClearTextBackColorPickers() {
        text_back_color_pickers_.Clear();
    }

    const InlineObjectEnumerator& InlineObjects() const {
        return inline_objects_.Items();
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

    void ReplaceSpan(const Range& span_range, std::size_t new_length);

    void Clear();

    RangedTextStyle Clone() const;

private:
    internal::RangedValueStore fonts_;
    internal::RangedValueStore text_color_pickers_;
    internal::RangedValueStore text_back_color_pickers_;
    InlineObjectStore inline_objects_;
};

}