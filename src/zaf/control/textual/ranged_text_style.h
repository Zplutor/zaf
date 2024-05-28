#pragma once

#include <optional>
#include <zaf/base/non_copyable.h>
#include <zaf/control/textual/inline_object_store.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/font/font.h>
#include <zaf/internal/ranged_value_store.h>

namespace zaf::textual {

class BaseRangedItem : NonCopyableNonMovable {
public:
    explicit BaseRangedItem(internal::RangedValueStore::Item& inner) : inner_(inner) {

    }

    explicit BaseRangedItem(const internal::RangedValueStore::Item& inner) : 
        inner_(const_cast<internal::RangedValueStore::Item&>(inner)) {

    }

    const Range& Range() const {
        return inner_.Range();
    }

protected:
    internal::RangedValueStore::Item& inner_;
};


class RangedFontItem : public BaseRangedItem {
public:
    using BaseRangedItem::BaseRangedItem;

    const zaf::Font& Font() const {
        return *std::any_cast<zaf::Font>(&this->inner_.Value());
    }

    zaf::Font& Font() {
        return const_cast<zaf::Font&>(static_cast<const RangedFontItem*>(this)->Font());
    }
};


class RangedColorItem : public BaseRangedItem {
public:
    using BaseRangedItem::BaseRangedItem;

    const zaf::Color& Color() const {
        return *std::any_cast<zaf::Color>(&this->inner_.Value());
    }

    zaf::Color& Color() {
        return const_cast<zaf::Color&>(static_cast<const RangedColorItem*>(this)->Color());
    }
};


template<typename StoreIterator, typename Item>
class RangedItemIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Item;
    using difference_type = std::make_signed_t<std::size_t>;

    using pointer = std::conditional_t<
        std::is_const_v<std::remove_pointer_t<typename StoreIterator::pointer>>,
        const value_type*, 
        value_type*>;

    using reference = std::conditional_t<
        std::is_const_v<std::remove_reference_t<typename StoreIterator::reference>>,
        const value_type&,
        value_type&>;

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


template<typename Item>
class RangedItemAccessor : NonCopyableNonMovable {
public:
    using value_type = Item;
    using iterator = RangedItemIterator<internal::RangedValueStore::iterator, Item>;
    using const_iterator = RangedItemIterator<internal::RangedValueStore::const_iterator, Item>;

public:
    explicit RangedItemAccessor(internal::RangedValueStore& store) noexcept : store_(store) {

    }

    std::size_t Count() const noexcept {
        return store_.Count();
    }

    bool IsEmpty() const noexcept {
        return store_.IsEmpty();
    }

    iterator FindItemAtIndex(std::size_t index) {
        auto inner_iterator = store_.FindItemAtIndex(index);
        if (inner_iterator != store_.end()) {
            return iterator{ inner_iterator };
        }
        return this->end();
    }

    const_iterator FindItemAtIndex(std::size_t index) const {
        auto inner_iterator = store_.FindItemAtIndex(index);
        if (inner_iterator != store_.end()) {
            return const_iterator{ inner_iterator };
        }
        return this->end();
    }

    void VisitItemsInRange(
        const Range& range,
        const std::function<void(const Item&)>& visitor) const {

        for (const auto& each_item : store_) {

            if (each_item.Range().EndIndex() <= range.index) {
                continue;
            }

            if (each_item.Range().index >= range.EndIndex()) {
                break;
            }

            visitor(Item{ each_item });
        }
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
    internal::RangedValueStore& store_;
};


class InlineObjectAccessor : NonCopyableNonMovable {
public:
    using iterator = InlineObjectStore::ItemList::iterator;
    using const_iterator = InlineObjectStore::ItemList::const_iterator;

public:
    explicit InlineObjectAccessor(const InlineObjectStore& store) noexcept : store_(store) {

    }

    bool IsEmpty() const noexcept {
        return store_.Items().empty();
    }

    const_iterator FindItemAtIndex(std::size_t index) const {
        return store_.FindItemAtIndex(index);
    }

    void VisitItemsInRange(
        const Range& range,
        const std::function<void(const InlineObjectStore::Item&)>& visitor) const {

        for (const auto& each_item : store_.Items()) {

            if (each_item.Range().EndIndex() <= range.index) {
                continue;
            }

            if (each_item.Range().index >= range.EndIndex()) {
                break;
            }

            if (range.Contains(each_item.Range())) {
                visitor(each_item);
            }
        }
    }

    const_iterator begin() const noexcept {
        return store_.Items().begin();
    }

    const_iterator end() const noexcept {
        return store_.Items().end();
    }

    const_iterator cbegin() const noexcept {
        return store_.Items().cbegin();
    }

    const_iterator cend() const noexcept {
        return store_.Items().cend();
    }

private:
    const InlineObjectStore& store_;
};


class RangedTextStyle : NonCopyable {
public:
    using FontAccessor = RangedItemAccessor<RangedFontItem>;
    using ColorAccessor = RangedItemAccessor<RangedColorItem>;

public:
    RangedTextStyle() = default;

    RangedTextStyle(RangedTextStyle&& other);
    RangedTextStyle& operator=(RangedTextStyle&& other);

    const FontAccessor& Fonts() const {
        return font_accessor_;
    }

    FontAccessor& Fonts() {
        return font_accessor_;
    }

    void SetFontInRange(Font font, const Range& range) {
        fonts_.AddRange(range, std::move(font));
    }

    void ClearFonts() {
        fonts_.Clear();
    }

    const ColorAccessor& TextColors() const {
        return text_color_accessor_;
    }

    ColorAccessor& TextColors() {
        return text_color_accessor_;
    }

    void SetTextColorInRange(const Color& color, const Range& range) {
        text_colors_.AddRange(range, color);
    }

    void ClearTextColors() {
        text_colors_.Clear();
    }

    const ColorAccessor& TextBackColors() const {
        return text_back_color_accessor_;
    }

    ColorAccessor& TextBackColors() {
        return text_back_color_accessor_;
    }

    void SetTextBackColorInRange(const Color& color, const Range& range) {
        text_back_colors_.AddRange(range, color);
    }

    void ClearTextBackColors() {
        text_back_colors_.Clear();
    }

    const InlineObjectAccessor& InlineObjects() const {
        return inline_object_accessor_;
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
    FontAccessor font_accessor_{ fonts_ };

    internal::RangedValueStore text_colors_;
    ColorAccessor text_color_accessor_{ text_colors_ };

    internal::RangedValueStore text_back_colors_;
    ColorAccessor text_back_color_accessor_{ text_back_colors_ };

    InlineObjectStore inline_objects_;
    InlineObjectAccessor inline_object_accessor_{ inline_objects_ };
};

}