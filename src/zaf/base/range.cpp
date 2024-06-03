#include <zaf/base/range.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/base/string/to_string.h>
#include <zaf/object/type_definition.h>
#include <zaf/xml/xml_error.h>
#include <zaf/xml/xml_reader.h>
#include <zaf/xml/xml_writer.h>

namespace zaf {

ZAF_DEFINE_TYPE(Range)
ZAF_DEFINE_TYPE_PROPERTY(Index)
ZAF_DEFINE_TYPE_PROPERTY(Length)
ZAF_DEFINE_TYPE_END

ZAF_DEFINE_EQUALITY(Range);

std::wstring Range::ToString() const {
    return ToWideString(Index()) + L',' + ToWideString(Length());
}


void Range::WriteToXML(XMLWriter& writer) const {

    writer.WriteElementStart(L"Range");
    writer.WriteAttribute(L"Index", std::to_wstring(index));
    writer.WriteAttribute(L"Length", std::to_wstring(length));
    writer.WriteElementEnd();
}


void Range::ReadFromXML(XMLReader& reader) {

    std::optional<std::size_t> xml_index;
    std::optional<std::size_t> xml_length;

    reader.ReadElementAttributes(
        L"Range", 
        [&xml_index, &xml_length](const auto& attribute_reader) {
    
            auto attribute_name = attribute_reader.GetName();
            if (attribute_name == L"Index") {
                xml_index = attribute_reader.GetNumber<std::size_t>();
            }
            else if (attribute_name == L"Length") {
                xml_length = attribute_reader.GetNumber<std::size_t>();
            }
        });

    if (!xml_index || !xml_length) {
        throw XMLError{ ZAF_SOURCE_LOCATION() };
    }

    this->index = *xml_index;
    this->length = *xml_length;
}

}