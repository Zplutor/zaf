#include <zaf/parsing/xaml_reader.h>

namespace zaf {
namespace {

std::wstring GetNodeName(IXmlReader* handle) {

    const wchar_t* name{};
    UINT name_length{};
    auto result = handle->GetLocalName(&name, &name_length);
    if (FAILED(result)) {
        return {};
    }
    return std::wstring(name, name_length);
}

}

XamlReader::XamlReader(IXmlReader* handle) : handle_(handle) {

}


XamlReader::~XamlReader() {
    if (handle_ != nullptr) {
        handle_->Release();
    }
}


XamlReader::XamlReader(XamlReader&& other) : handle_(other.handle_) {
    other.handle_ = nullptr;
}


XamlReader& XamlReader::operator=(XamlReader&& other) {
    handle_ = other.handle_;
    other.handle_ = nullptr;
    return *this;
}


bool XamlReader::Next() {

    while (true) {

        XmlNodeType node_type{};
        auto result = handle_->Read(&node_type);
        if (FAILED(result)) {
            return false;
        }

        switch (node_type) {
        case XmlNodeType::XmlNodeType_Element:
        case XmlNodeType::XmlNodeType_EndElement:
        case XmlNodeType::XmlNodeType_Text:
            return true;
        }
    }

    return false;
}


XamlNodeType XamlReader::GetNodeType() const {

    XmlNodeType node_type{};
    auto result = handle_->GetNodeType(&node_type);
    if (FAILED(result)) {
        return XamlNodeType::None;
    }

    switch (node_type) {
    case XmlNodeType::XmlNodeType_Element:
        return XamlNodeType::Element;
    case XmlNodeType::XmlNodeType_EndElement:
        return XamlNodeType::EndElement;
    case XmlNodeType::XmlNodeType_Text:
        return XamlNodeType::Text;
    default:
        return XamlNodeType::None;
    }
}


std::wstring XamlReader::GetName() const {
    return GetNodeName(handle_);
}


XamlReader::AttributeReader XamlReader::GetAttributeReader() const {
    return AttributeReader(handle_);
}


XamlReader::AttributeReader::AttributeReader(IXmlReader* handle) : handle_(handle) {

}


XamlReader::AttributeReader::AttributeReader(AttributeReader&& other) : handle_(other.handle_) {
    other.handle_ = nullptr;
}


XamlReader::AttributeReader& XamlReader::AttributeReader::operator=(AttributeReader&& other) {
    handle_ = other.handle_;
    other.handle_ = nullptr;
    return *this;
}


bool XamlReader::AttributeReader::Read() {
    return SUCCEEDED(handle_->MoveToNextAttribute());
}


std::wstring XamlReader::AttributeReader::GetName() const {
    return GetNodeName(handle_);
}


std::wstring XamlReader::AttributeReader::GetValue() const {

    const wchar_t* value{};
    UINT value_length{};
    auto result = handle_->GetLocalName(&value, &value_length);
    if (FAILED(result)) {
        return {};
    }
    return std::wstring(value, value_length);
}

}