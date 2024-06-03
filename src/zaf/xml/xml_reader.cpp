#include <zaf/xml/xml_reader.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/error/contract_error.h>

#include <zaf/xml/xml_error.h>

namespace zaf {
namespace {

COMPtr<IXmlReader> CreateIXmlReader() {
    COMPtr<IXmlReader> reader;
    HRESULT hresult = CreateXmlReader(__uuidof(IXmlReader), reader.ResetAsVoid(), nullptr);
    ZAF_THROW_IF_COM_ERROR(hresult);
    return reader;
}


bool IsContentNodeType(XMLNodeType node_type) {
    return
        node_type == XMLNodeType::XMLDeclaration ||
        node_type == XMLNodeType::ElementStart ||
        node_type == XMLNodeType::ElementEnd ||
        node_type == XMLNodeType::Attribute ||
        node_type == XMLNodeType::CDATA ||
        node_type == XMLNodeType::Text;
};

}

XMLReader::XMLReader(Stream stream) {

    ZAF_EXPECT(stream);

    inner_ = CreateIXmlReader();

    HRESULT hresult = inner_->SetInput(stream.Ptr().Inner());
    ZAF_THROW_IF_COM_ERROR(hresult);
}


XMLReader::XMLReader(Stream stream, CodePage code_page) {

    ZAF_EXPECT(stream);

    COMPtr<IXmlReaderInput> input;
    HRESULT hresult = CreateXmlReaderInputWithEncodingCodePage(
        stream.Ptr().Inner(),
        nullptr,
        static_cast<UINT>(code_page),
        FALSE,
        nullptr,
        input.Reset());

    ZAF_THROW_IF_COM_ERROR(hresult);

    inner_ = CreateIXmlReader();
    inner_->SetInput(input.Inner());
}


XMLNodeType XMLReader::GetNodeType() const {

    XmlNodeType node_type{};
    HRESULT hresult = inner_->GetNodeType(&node_type);
    return static_cast<XMLNodeType>(node_type);
}


std::wstring_view XMLReader::GetName() const {

    const wchar_t* name{};
    UINT length{};
    HRESULT hresult = inner_->GetLocalName(&name, &length);
    ZAF_THROW_IF_COM_ERROR(hresult);

    return { name, length };
}


std::wstring_view XMLReader::GetValue() const {

    const wchar_t* value{};
    UINT length{};
    HRESULT hresult = inner_->GetValue(&value, &length);
    ZAF_THROW_IF_COM_ERROR(hresult);

    return { value, length };
}


bool XMLReader::Read() {

    HRESULT hresult = inner_->Read(nullptr);
    ZAF_THROW_IF_COM_ERROR(hresult);

    return hresult == S_OK;
}


void XMLReader::ReadXMLDeclaration() {
    ReadNode(XMLNodeType::XMLDeclaration, std::nullopt, true);
}


void XMLReader::ReadUntilElementStart(std::wstring_view element_name) {
    ReadNode(XMLNodeType::ElementStart, element_name, false);
}


void XMLReader::ReadElementStart(std::wstring_view element_name) {
    ReadNode(XMLNodeType::ElementStart, element_name, true);
}


bool XMLReader::TryReadElementStart(std::wstring_view element_name) {
    return TryReadNode(XMLNodeType::ElementStart, element_name, true);
}


void XMLReader::ReadElementEnd() {
    ReadNode(XMLNodeType::ElementEnd, std::nullopt, true);
}


void XMLReader::ReadElementAttributes(
    std::wstring_view element_name,
    const std::function<void(const XMLAttributeReader&)>& visitor) {

    ZAF_EXPECT(visitor);

    ReadUntilElementStart(element_name);

    if (MoveToFirstAttribute()) {
        do {
            visitor(XMLAttributeReader{ *this });
        } while (MoveToNextAttribute());
        MoveToElement();
    }

    BOOL is_empty_element = inner_->IsEmptyElement();
    Read();
    if (!is_empty_element) {
        ReadElementEnd();
    }
}


std::wstring_view XMLReader::ReadCDATA() {

    ReadUntilContent();

    if (GetNodeType() != XMLNodeType::CDATA) {
        throw XMLError{ ZAF_SOURCE_LOCATION() };
    }

    return GetValue();
}


void XMLReader::ReadUntilContent() {

    while (!IsContentNodeType(GetNodeType())) {
        if (!Read()) {
            break;
        }
    }
}


void XMLReader::ReadNode(
    XMLNodeType node_type,
    std::optional<std::wstring_view> node_name, 
    bool skip) {

    if (!TryReadNode(node_type, node_name, skip)) {
        throw XMLError{ ZAF_SOURCE_LOCATION() };
    }
}


bool XMLReader::TryReadNode(
    XMLNodeType node_type, 
    std::optional<std::wstring_view> node_name, 
    bool skip) {

    ReadUntilContent();

    if (GetNodeType() != node_type) {
        return false;
    }

    if (node_name) {
        if (node_name != GetName()) {
            return false;
        }
    }

    if (skip) {
        Read();
    }
    return true;
}


bool XMLReader::MoveToFirstAttribute() {

    HRESULT hresult = inner_->MoveToFirstAttribute();
    ZAF_THROW_IF_COM_ERROR(hresult);

    return hresult == S_OK;
}


bool XMLReader::MoveToNextAttribute() {

    HRESULT hresult = inner_->MoveToNextAttribute();
    ZAF_THROW_IF_COM_ERROR(hresult);

    return hresult == S_OK;
}


void XMLReader::MoveToElement() {

    HRESULT hresult = inner_->MoveToElement();
    ZAF_THROW_IF_COM_ERROR(hresult);
}

}