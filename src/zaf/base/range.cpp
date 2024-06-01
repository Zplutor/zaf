#include <zaf/base/range.h>
#include <zaf/base/string/to_string.h>
#include <zaf/object/type_definition.h>
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

}