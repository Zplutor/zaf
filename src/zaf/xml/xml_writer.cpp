#include <zaf/xml/xml_writer.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/error/contract_error.h>

namespace zaf {

XMLWriter::XMLWriter(Stream stream) {

    ZAF_EXPECT(stream);

    HRESULT hresult = CreateXmlWriter(IID_IXmlWriter, inner_.ResetAsVoid(), nullptr);
    ZAF_THROW_IF_COM_ERROR(hresult);

    hresult = inner_->SetOutput(stream.Ptr().Inner());
    ZAF_THROW_IF_COM_ERROR(hresult);
}


void XMLWriter::WriteDocumentStart() {

    HRESULT hresult = inner_->WriteStartDocument(XmlStandalone_Yes);
    ZAF_THROW_IF_COM_ERROR(hresult);
}


void XMLWriter::WriteDocumentEnd() {

    HRESULT hresult = inner_->WriteEndDocument();
    ZAF_THROW_IF_COM_ERROR(hresult);
}


void XMLWriter::WriteElementStart(const std::wstring& name) {

    HRESULT hresult = inner_->WriteStartElement(nullptr, name.c_str(), nullptr);
    ZAF_THROW_IF_COM_ERROR(hresult);
}


void XMLWriter::WriteElementEnd() {

    HRESULT hresult = inner_->WriteEndElement();
    ZAF_THROW_IF_COM_ERROR(hresult);
}


void XMLWriter::WriteString(const std::wstring& string) {

    HRESULT hresult = inner_->WriteString(string.c_str());
    ZAF_THROW_IF_COM_ERROR(hresult);
}


void XMLWriter::WriteCDATA(const std::wstring& string) {

    HRESULT hresult = inner_->WriteCData(string.c_str());
    ZAF_THROW_IF_COM_ERROR(hresult);
}


void XMLWriter::WriteAttribute(const std::wstring& name, const std::wstring& value) {

    HRESULT hresult = inner_->WriteAttributeString(nullptr, name.c_str(), nullptr, value.c_str());
    ZAF_THROW_IF_COM_ERROR(hresult);
}

}
