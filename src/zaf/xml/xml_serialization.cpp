#include <zaf/xml/xml_serialization.h>
#include <zaf/base/stream.h>
#include <zaf/base/string/encoding_conversion.h>
#include <zaf/xml/xml_reader.h>
#include <zaf/xml/xml_writer.h>

namespace zaf {

void XMLSerialize(
    const XMLSerializable& serializable,
    Stream stream,
    const XMLOutputOptions& options) {

    XMLWriter writer{ std::move(stream), options };
    writer.WriteDocumentStart();
    serializable.WriteToXML(writer);
    writer.WriteDocumentEnd();
}


Stream XMLSerializeToMemoryStream(
    const XMLSerializable& serializable,
    const XMLOutputOptions& options) {

    auto stream = Stream::FromMemory(0);
    XMLSerialize(serializable, stream, options);
    return stream;
}


std::wstring XMLSerializeToText(const XMLSerializable& serializable) {

    auto stream = XMLSerializeToMemoryStream(serializable, {
        .code_page = CodePage::UTF16,
        .use_bom = false 
    });

    auto buffer = stream.GetUnderlyingBuffer();
    if (!buffer) {
        return {};
    }

    return std::wstring{ 
        reinterpret_cast<const wchar_t*>(buffer),
        stream.GetSize() / sizeof(wchar_t) 
    };
}


void XMLDeserialize(
    XMLSerializable& serializable,
    const Stream& stream,
    const XMLInputOptions& options) {

    XMLReader reader{ stream };

    reader.ReadXMLDeclaration();
    serializable.ReadFromXML(reader);
}


void XMLDeserialize(XMLSerializable& serializable, std::wstring_view text) {

    auto stream = Stream::CreateOnMemory(text.data(), text.length() * sizeof(wchar_t));
    XMLReader reader{ std::move(stream), { .code_page = CodePage::UTF16 } };

    reader.ReadXMLDeclaration();
    serializable.ReadFromXML(reader);
}

}