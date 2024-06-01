#include <zaf/control/textual/default_text_style.h>
#include <zaf/xml/xml_writer.h>

namespace zaf::textual {

void DefaultTextStyle::WriteToXML(XMLWriter& writer) const {

    writer.WriteElementStart(L"DefaultTextStyle");

    writer.WriteElementStart(L"Font");
    font_.WriteToXML(writer);
    writer.WriteElementEnd();

    writer.WriteElementStart(L"TextColor");
    text_color_.WriteToXML(writer);
    writer.WriteElementEnd();

    writer.WriteElementStart(L"TextBackColor");
    text_back_color_.WriteToXML(writer);
    writer.WriteElementEnd();

    writer.WriteElementEnd();
}

}