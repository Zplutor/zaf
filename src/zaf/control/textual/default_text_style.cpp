#include "default_text_style.h"
#include <zaf/control/textual/default_text_style.h>
#include <zaf/xml/xml_reader.h>
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


void DefaultTextStyle::ReadFromXML(XMLReader& reader) {

    reader.ReadNotEmptyElementStart(L"DefaultTextStyle");

    reader.ReadNotEmptyElementStart(L"Font");
    font_.ReadFromXML(reader);
    reader.ReadElementEnd();

    reader.ReadNotEmptyElementStart(L"TextColor");
    text_color_.ReadFromXML(reader);
    reader.ReadElementEnd();

    reader.ReadNotEmptyElementStart(L"TextBackColor");
    text_back_color_.ReadFromXML(reader);
    reader.ReadElementEnd();

    reader.ReadElementEnd();
}

}