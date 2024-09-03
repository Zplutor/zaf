#pragma once

#include <optional>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range_map.h>
#include <zaf/control/textual/inline_object_store.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/font/font.h>
#include <zaf/xml/xml_serializable.h>

namespace zaf::textual {

template<typename T>
class RangedItemAccessor : NonCopyableNonMovable {
public:
    using value_type = RangeMap<T>::Item;
    using iterator = RangeMap<T>::iterator;
    using const_iterator = RangeMap<T>::const_iterator;

public:
    explicit RangedItemAccessor(RangeMap<T>& map) noexcept : map_(map) {

    }

    std::size_t Count() const noexcept {
        return map_.Count();
    }

    bool IsEmpty() const noexcept {
        return map_.IsEmpty();
    }

    iterator FindItemAtIndex(std::size_t index) {
        return map_.FindItemAtIndex(index);
    }

    const_iterator FindItemAtIndex(std::size_t index) const {
        return map_.FindItemAtIndex(index);
    }

    iterator FindFirstItemIntersectsWithRange(const Range& range) {
        return map_.FindFirstItemIntersectsWithRange(range);
    }

    const_iterator FindFirstItemIntersectsWithRange(const Range& range) const {
        return map_.FindFirstItemIntersectsWithRange(range);
    }

    void VisitItemsInRange(
        const Range& range,
        const std::function<void(const value_type&)>& visitor) const {

        for (const auto& each_item : map_) {

            if (each_item.Range().EndIndex() <= range.index) {
                continue;
            }

            if (each_item.Range().index >= range.EndIndex()) {
                break;
            }

            visitor(each_item);
        }
    }

    iterator begin() noexcept {
        return iterator{ map_.begin() };
    }

    iterator end() noexcept {
        return iterator{ map_.end() };
    }

    const_iterator begin() const noexcept {
        return const_iterator{ map_.begin() };
    }

    const_iterator end() const noexcept {
        return const_iterator{ map_.end() };
    }

    const_iterator cbegin() const noexcept {
        return const_iterator{ map_.begin() };
    }

    const_iterator cend() const noexcept {
        return const_iterator{ map_.end() };
    }

private:
    RangeMap<T>& map_;
};


class InlineObjectAccessor : NonCopyableNonMovable {
public:
    using value_type = InlineObjectStore::Item;
    using iterator = InlineObjectStore::ItemList::iterator;
    using const_iterator = InlineObjectStore::ItemList::const_iterator;

public:
    explicit InlineObjectAccessor(const InlineObjectStore& store) noexcept : store_(store) {

    }

    bool IsEmpty() const noexcept {
        return store_.Items().empty();
    }

    std::size_t Count() const noexcept {
        return store_.Items().size();
    }

    const_iterator FindItemAtIndex(std::size_t index) const {
        return store_.FindItemAtIndex(index);
    }

    const_iterator FindFirstItemIntersectsWithRange(const Range& range) const {
        return store_.FindFirstItemIntersectsWithRange(range);
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


class RangedTextStyle : public XMLSerializable, NonCopyable {
public:
    using FontAccessor = RangedItemAccessor<Font>;
    using ColorAccessor = RangedItemAccessor<Color>;

public:
    RangedTextStyle() noexcept = default;

    RangedTextStyle(RangedTextStyle&& other) noexcept;
    RangedTextStyle& operator=(RangedTextStyle&& other) noexcept;

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

    void WriteToXML(XMLWriter& writer) const override;
    void ReadFromXML(XMLReader& reader) override;

private:
    void WriteInlineObjectsToXML(XMLWriter& writer) const;
    void ReadInlineObjectsFromXML(XMLReader& reader);

private:
    RangeMap<Font> fonts_;
    FontAccessor font_accessor_{ fonts_ };

    RangeMap<Color> text_colors_;
    ColorAccessor text_color_accessor_{ text_colors_ };

    RangeMap<Color> text_back_colors_;
    ColorAccessor text_back_color_accessor_{ text_back_colors_ };

    InlineObjectStore inline_objects_;
    InlineObjectAccessor inline_object_accessor_{ inline_objects_ };
};

}