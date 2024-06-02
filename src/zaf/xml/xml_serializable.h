#pragma once

namespace zaf {

class XMLReader;
class XMLWriter;

class XMLSerializable {
public:
    XMLSerializable() = default;
    virtual ~XMLSerializable() = default;

    virtual void WriteToXML(XMLWriter& writer) const = 0;
    virtual void ReadFromXML(XMLReader& reader) = 0;
};

}