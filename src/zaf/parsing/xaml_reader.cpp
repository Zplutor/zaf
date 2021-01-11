#include <zaf/parsing/xaml_reader.h>
#include <atlbase.h>
#include <Shlwapi.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/stream.h>
#include <zaf/base/string/encoding_conversion.h>

namespace zaf {
namespace {

IXmlReader* CreateHandle(IUnknown* input) {

    CComPtr<IXmlReader> handle;
    HRESULT result = CreateXmlReader(
        __uuidof(IXmlReader),
        reinterpret_cast<void**>(&handle), 
        nullptr);

    ZAF_THROW_IF_COM_ERROR(result);

    result = handle->SetInput(input);

    ZAF_THROW_IF_COM_ERROR(result);
    return handle.Detach();
}


std::shared_ptr<XamlReader> CreateXamlReaderFromMemory(const void* data, std::size_t length) {

    CComPtr<IStream> stream = SHCreateMemStream(reinterpret_cast<const BYTE*>(data), length);
    if (stream == nullptr) {
        ZAF_THROW_IF_COM_ERROR(E_OUTOFMEMORY);
    }

    auto handle = CreateHandle(stream.p);

    stream.Detach();
    return std::make_shared<XamlReader>(handle);
}

}


std::shared_ptr<XamlReader> XamlReader::FromString(const std::wstring& xaml) {
    return FromString(ToUtf8String(xaml));
}


std::shared_ptr<XamlReader> XamlReader::FromString(const std::string& xaml) {
    return CreateXamlReaderFromMemory(xaml.data(), xaml.length());
}


std::shared_ptr<XamlReader> XamlReader::FromStream(const Stream& stream) {

    auto stream_handle = stream.GetHandle();
    auto xaml_reader_handle = CreateHandle(stream_handle);

    stream_handle->AddRef();
    return std::make_shared<XamlReader>(xaml_reader_handle);
}


XamlReader::XamlReader(IXmlReader* handle) : handle_(handle) {

}


XamlReader::~XamlReader() {
    handle_->Release();
}


std::shared_ptr<XamlNode> XamlReader::Read() {

    std::shared_ptr<XamlNode> root_node;
    HRESULT result = ReadRootNode(root_node);

    ZAF_THROW_IF_COM_ERROR(result);
    return root_node;
}


HRESULT XamlReader::ReadRootNode(std::shared_ptr<XamlNode>& root_node) {

    XmlNodeType xml_node_type{};
    auto result = AdvanceToNextNode(xml_node_type);
    if (result != S_OK) {
        return result;
    }

    if (xml_node_type != XmlNodeType_Element) {
        return E_INVALIDARG;
    }

    return ReadElementNode(root_node);
}


HRESULT XamlReader::ReadElementNode(std::shared_ptr<XamlNode>& node) {

    node = std::make_shared<XamlNode>(XamlNode::Type::Element);

    const wchar_t* name{};
    HRESULT result = handle_->GetLocalName(&name, nullptr);
    if (result != S_OK) {
        return result;
    }

    node->SetValue(name);

    result = ReadAttributes(*node);
    if (FAILED(result)) {
        return result;
    }

    return ReadChildren(*node);
}


HRESULT XamlReader::ReadAttributes(XamlNode& node) {

    HRESULT result = S_OK;
    bool has_attributes{};
    while (true) {

        result = handle_->MoveToNextAttribute();
        if (result != S_OK) {
            break;
        }

        const wchar_t* name = nullptr;
        result = handle_->GetLocalName(&name, nullptr);
        if (result != S_OK) {
            break;
        }

        const wchar_t* value = nullptr;
        result = handle_->GetValue(&value, nullptr);
        if (result != S_OK) {
            break;
        }

        node.AddAttribute(name, value);
        has_attributes = true;
    }

    if (has_attributes) {
        result = handle_->MoveToElement();
    }

    return result;
}


HRESULT XamlReader::ReadChildren(XamlNode& node) {

    if (handle_->IsEmptyElement()) {
        return S_OK;
    }

    HRESULT result = S_OK;
    while (true) {

        XmlNodeType node_type;
        result = AdvanceToNextNode(node_type);
        if (result != S_OK) {
            break;
        }

        if (node_type == XmlNodeType_Element) {

            std::shared_ptr<XamlNode> child_node;
            result = ReadElementNode(child_node);
            if (result != S_OK) {
                break;
            }

            node.AddChildNode(child_node);
        }
        else if (node_type == XmlNodeType_Text) {

            std::shared_ptr<XamlNode> child_node;
            result = ReadTextNode(child_node);
            if (result != S_OK) {
                break;
            }

            node.AddChildNode(child_node);
        }
        else if (node_type == XmlNodeType_EndElement) {
            break;
        }
        else {
            result = E_INVALIDARG;
            break;
        }
    }

    return result;
}


HRESULT XamlReader::ReadTextNode(std::shared_ptr<XamlNode>& node) {

    node = std::make_shared<XamlNode>(XamlNode::Type::Text);

    const wchar_t* value{};
    HRESULT result = handle_->GetValue(&value, nullptr);
    if (result == S_OK) {
        node->SetValue(value);
    }
    return result;
}


HRESULT XamlReader::AdvanceToNextNode(XmlNodeType& next_node_type) {

    HRESULT result = S_OK;
    while (true) {

        result = handle_->Read(&next_node_type);
        if (result != S_OK) {
            break;
        }

        if (next_node_type != XmlNodeType_Comment &&
            next_node_type != XmlNodeType_Whitespace) {
            break;
        }
    }

    return result;
}

}