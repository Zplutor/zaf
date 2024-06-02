#include <zaf/xml/xml_reader.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/xml/xml_error.h>

namespace zaf {
namespace {

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

    HRESULT result = CreateXmlReader(__uuidof(IXmlReader), inner_.ResetAsVoid(), nullptr);
    ZAF_THROW_IF_COM_ERROR(result);

    result = inner_->SetInput(stream.Ptr().Inner());
    ZAF_THROW_IF_COM_ERROR(result);
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

    XmlNodeType node_type{};
    HRESULT hresult = inner_->Read(&node_type);
    ZAF_THROW_IF_COM_ERROR(hresult);

    if (hresult == S_OK) {
        current_node_type_ = static_cast<XMLNodeType>(node_type);
        return true;
    }
    else {
        current_node_type_ = XMLNodeType::None;
        return false;
    }
}


void XMLReader::ReadUntilElementStart(std::wstring_view element_name) {
    ReadNode(XMLNodeType::ElementStart, element_name, false);
}


void XMLReader::ReadElementStart(std::wstring_view element_name) {
    ReadNode(XMLNodeType::ElementStart, element_name, true);
}


void XMLReader::ReadElementEnd() {
    ReadNode(XMLNodeType::ElementEnd, std::nullopt, true);
}


void XMLReader::ReadXMLDeclaration() {
    ReadNode(XMLNodeType::XMLDeclaration, std::nullopt, true);
}


void XMLReader::ReadUntilContent() {

    while (!IsContentNodeType(current_node_type_)) {
        if (!Read()) {
            break;
        }
    }
}


void XMLReader::ReadNode(
    XMLNodeType node_type,
    std::optional<std::wstring_view> node_name, 
    bool skip) {

    ReadUntilContent();

    if (current_node_type_ != node_type) {
        throw XMLError{ ZAF_SOURCE_LOCATION() };
    }

    if (node_name) {
        if (node_name != GetName()) {
            throw XMLError{ ZAF_SOURCE_LOCATION() };
        }
    }

    if (skip) {
        Read();
    }
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

}