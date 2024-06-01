#pragma once

namespace zaf {

class XMLWriter;

class XMLSerializable {
public:
    XMLSerializable() = default;
    virtual ~XMLSerializable() = default;

    virtual void WriteToXML(XMLWriter& writer) const = 0;
};

}