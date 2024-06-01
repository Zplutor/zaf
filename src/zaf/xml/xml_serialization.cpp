#include <zaf/xml/xml_serialization.h>
#include <zaf/base/stream.h>
#include <zaf/base/string/encoding_conversion.h>
#include <zaf/xml/xml_writer.h>

namespace zaf {

std::wstring XMLSerializeToText(const XMLSerializable& serializable) {

    auto stream = Stream::FromMemory(0);
    {
        XMLWriter writer{ stream };
        writer.WriteDocumentStart();
        serializable.WriteToXML(writer);
        writer.WriteDocumentEnd();
    }

    auto buffer = stream.GetUnderlyingBuffer();
    if (!buffer) {
        return {};
    }

    std::u8string_view string_view{ reinterpret_cast<const char8_t*>(buffer), stream.GetSize() };
    return FromUTF8String(string_view);
}

}