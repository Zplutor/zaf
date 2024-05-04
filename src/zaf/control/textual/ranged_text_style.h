#pragma once

#include <optional>
#include <zaf/base/error/contract_error.h>
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

    bool operator==(const RangedItemIterator& other) const noexcept {
        return inner_ == other.inner_;
    }

    bool operator!=(const RangedItemIterator& other) const noexcept {
        return inner_ != other.inner_;
    }

private:
    StoreIterator inner_;
    mutable std::optional<Item> temp_item_;
};


template<bool IsConst, template<bool> typename Item>
class RangedItemAccessor {
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
    explicit RangedItemAccessor(StoreType store) noexcept : store_(store) {

    }

    bool IsEmpty() const noexcept {
        return store_.IsEmpty();
    }

    iterator FindItemContainsIndex(std::size_t index) {
        auto inner_iterator = store_.FindItemContainsIndex(index);
        if (inner_iterator != store_.end()) {
            return iterator{ inner_iterator };
        }
        return this->end();
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

private:
    StoreType store_;
};


class InlineObjectAccessor : NonCopyable {
public:
    using iterator = InlineObjectStore::ItemList::iterator;
    using const_iterator = InlineObjectStore::ItemList::const_iterator;

public:
    explicit InlineObjectAccessor(const InlineObjectStore& store) : store_(store) {

    }

    const_iterator FindItemContainsIndex(std::size_t index) const {
        return store_.FindItemContainsIndex(index);
    }

    const_iterator begin() const {
        return const_iterator{ store_.Items().begin() };
    }

    const_iterator end() const {
        return const_iterator{ store_.Items().end() };
    }

private:
    const InlineObjectStore& store_;
};


class RangedTextStyle : NonCopyable {
public:
    using FontAccessor = RangedItemAccessor<false, RangedFontItem>;
    using ConstFontAccessor = RangedItemAccessor<true, RangedFontItem>;

    using ColorPickerAccessor = RangedItemAccessor<false, RangedColorPickerItem>;
    using ConstColorPickerAccessor = RangedItemAccessor<true, RangedColorPickerItem>;

public:
    RangedTextStyle() = default;

    ConstFontAccessor Fonts() const {
        return ConstFontAccessor{ fonts_ };
    }

    FontAccessor Fonts() {
        return FontAccessor{ fonts_ };
    }

    void SetFontInRange(Font font, const Range& range) {
        fonts_.AddRange(range, std::move(font));
    }

    void ClearFonts() {
        fonts_.Clear();
    }

    ConstColorPickerAccessor TextColorPickers() const {
        return ConstColorPickerAccessor{ text_color_pickers_ };
    }

    ColorPickerAccessor TextColorPickers() {
        return ColorPickerAccessor{ text_color_pickers_ };
    }

    void SetTextColorPickerInRange(ColorPicker color_picker, const Range& range) {
        ZAF_EXPECT(color_picker);
        text_color_pickers_.AddRange(range, std::move(color_picker));
    }

    void ClearTextColorPickers() {
        text_color_pickers_.Clear();
    }

    ConstColorPickerAccessor TextBackColorPickers() const {
        return ConstColorPickerAccessor{ text_back_color_pickers_ };
    }

    ColorPickerAccessor TextBackColorPickers() {
        return ColorPickerAccessor{ text_back_color_pickers_ };
    }

    void SetTextBackColorPickerInRange(ColorPicker color_picker, const Range& range) {
        ZAF_EXPECT(color_picker);
        text_back_color_pickers_.AddRange(range, std::move(color_picker));
    }

    void ClearTextBackColorPickers() {
        text_back_color_pickers_.Clear();
    }

    InlineObjectAccessor InlineObjects() const {
        return InlineObjectAccessor{ inline_objects_ };
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