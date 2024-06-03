#include "ranged_text_style.h"
#include <zaf/control/textual/ranged_text_style.h>
#include <zaf/xml/xml_reader.h>
#include <zaf/xml/xml_writer.h>

namespace zaf::textual {
namespace {

template<typename T>
void WriteRangedStyleToXML(
    const RangeMap<T>& map,
    const std::wstring& range_element_name, 
    const std::wstring& item_element_name,
    XMLWriter& writer) {

    writer.WriteElementStart(range_element_name);

    for (const auto& each_item : map) {

        writer.WriteElementStart(item_element_name);
        each_item.Range().WriteToXML(writer);
        each_item.Value().WriteToXML(writer);
        writer.WriteElementEnd();
    }

    writer.WriteElementEnd();
}


template<typename T>
void ReadRangedStyleFromXML(
    XMLReader& reader,
    std::wstring_view range_element_name,
    std::wstring_view item_element_name,
    RangeMap<T>& map) {

    reader.ReadElementStart(range_element_name);

    while (reader.TryReadElementStart(item_element_name)) {

        Range range;
        range.ReadFromXML(reader);

        T value;
        value.ReadFromXML(reader);

        map.AddRange(range, std::move(value));

        reader.ReadElementEnd();
    }

    reader.ReadElementEnd();
}

}

RangedTextStyle::RangedTextStyle(RangedTextStyle&& other) noexcept : 
    fonts_(std::move(other.fonts_)),
    text_colors_(std::move(other.text_colors_)),
    text_back_colors_(std::move(other.text_back_colors_)),
    inline_objects_(std::move(other.inline_objects_)) {

}


RangedTextStyle& RangedTextStyle::operator=(RangedTextStyle&& other) noexcept {

    this->fonts_ = std::move(other.fonts_);
    this->text_colors_ = std::move(other.text_colors_);
    this->text_back_colors_ = std::move(other.text_back_colors_);
    this->inline_objects_ = std::move(other.inline_objects_);
    return *this;
}


void RangedTextStyle::ReplaceSpan(const Range& span_range, std::size_t new_length) {

    fonts_.ReplaceSpan(span_range, new_length);
    text_colors_.ReplaceSpan(span_range, new_length);
    text_back_colors_.ReplaceSpan(span_range, new_length);
    inline_objects_.ReplaceSpan(span_range, new_length);
}


void RangedTextStyle::Clear() {
    fonts_.Clear();
    text_colors_.Clear();
    text_back_colors_.Clear();
    inline_objects_.Clear();
}


RangedTextStyle RangedTextStyle::Clone() const {

    RangedTextStyle result;
    result.fonts_ = this->fonts_;
    result.text_colors_ = this->text_colors_;
    result.text_back_colors_ = this->text_back_colors_;
    result.inline_objects_ = this->inline_objects_.Clone();
    return result;
}


void RangedTextStyle::WriteToXML(XMLWriter& writer) const {

    writer.WriteElementStart(L"RangedTextStyle");
    WriteRangedStyleToXML(fonts_, L"RangedFonts", L"RangedFontItem", writer);
    WriteRangedStyleToXML(text_colors_, L"RangedTextColors", L"RangedColorItem", writer);
    WriteRangedStyleToXML(text_back_colors_, L"RangedTextBackColors", L"RangedColorItem", writer);
    writer.WriteElementEnd();
}


void RangedTextStyle::ReadFromXML(XMLReader& reader) {

    reader.ReadElementStart(L"RangedTextStyle");

    ReadRangedStyleFromXML(reader, L"RangedFonts", L"RangedFontItem", fonts_);
    ReadRangedStyleFromXML(reader, L"RangedTextColors", L"RangedColorItem", text_colors_);
    ReadRangedStyleFromXML(reader, L"RangedTextBackColors", L"RangedColorItem", text_back_colors_);

    reader.ReadElementEnd();
}

}