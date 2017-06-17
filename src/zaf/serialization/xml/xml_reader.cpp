#include <zaf/serialization/xml/xml_reader.h>
#include <atlbase.h>
#include <xmllite.h>
#include <zaf/base/error.h>
#include <zaf/base/stream.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/xml/internal/xml_names.h>

namespace zaf {
namespace {

class DataNodeReader {
public:
    DataNodeReader(IXmlReader* reader) : reader_(reader) { }

    HRESULT Execute(std::shared_ptr<DataNode>& data_node) {

        XmlNodeType node_type;
        auto result = AdvanceToNextNode(node_type);
        if (result != S_OK) {
            return result;
        }

        if (node_type != XmlNodeType_Element) {
            return E_INVALIDARG;
        }

        std::wstring key;
        return ReadDataNode(data_node, key);
    }

private:
    HRESULT ReadDataNode(std::shared_ptr<DataNode>& data_node, std::wstring& key) {

        const wchar_t* name = nullptr;
        HRESULT result = reader_->GetLocalName(&name, nullptr);
        if (result != S_OK) {
            return result;
        }

        result = ReadKeyAttribute(key);
        if (FAILED(result)) {
            return result;
        }

        if (std::wcscmp(name, internal::StringElementName) == 0) {
            return ReadString(data_node);
        }
        else if (std::wcscmp(name, internal::BooleanElementName) == 0) {
            return ReadBoolean(data_node);
        }
        else if (std::wcscmp(name, internal::NumberElementName) == 0) {
            return ReadNumber(data_node);
        }
        else if (std::wcscmp(name, internal::ObjectElementName) == 0) {
            return ReadObject(data_node);
        }
        else if (std::wcscmp(name, internal::ArrayElementName) == 0) {
            return ReadArray(data_node);
        }
        else {
            return E_INVALIDARG;
        }
    }

    HRESULT ReadString(std::shared_ptr<DataNode>& data_node) {

        std::wstring text;
        HRESULT result = ReadElementText(text);
        if (result != S_OK) {
            return result;
        }

        data_node = DataNode::CreateString(text);
        return S_OK;
    }

    HRESULT ReadBoolean(std::shared_ptr<DataNode>& data_node) {

        std::wstring text;
        HRESULT result = ReadElementText(text);
        if (result != S_OK) {
            return result;
        }

        bool boolean = false;
        if (text == L"true") {
            boolean = true;
        }
        else if (text == L"false") {
            boolean = false;
        }
        else if (text.empty()) {
            boolean = false;
        }
        else {
            return E_INVALIDARG;
        }

        data_node = DataNode::CreateBoolean(boolean);
        return S_OK;
    }

    HRESULT ReadNumber(std::shared_ptr<DataNode>& data_node) {

        std::wstring text;
        HRESULT result = ReadElementText(text);
        if (result != S_OK) {
            return result;
        }

        if (text.empty()) {
            data_node = DataNode::CreateNumber(0);
            return S_OK;
        }

        std::int64_t signed_integer = 0;
        if (zaf::TryToNumeric<std::int64_t>(text, signed_integer)) {
            data_node = DataNode::CreateNumber(signed_integer);
            return S_OK;
        }

        std::uint64_t unsigned_integer = 0;
        if (zaf::TryToNumeric<std::uint64_t>(text, unsigned_integer)) {
            data_node = DataNode::CreateNumber(unsigned_integer);
            return S_OK;
        }

        double floating_point = 0;
        if (zaf::TryToNumeric<double>(text, floating_point)) {
            data_node = DataNode::CreateNumber(floating_point);
            return S_OK;
        }

        return E_INVALIDARG;
    }

    HRESULT ReadObject(std::shared_ptr<DataNode>& data_node) {

        std::vector<std::pair<std::wstring, std::shared_ptr<DataNode>>> child_data_nodes;
        HRESULT result = ReadChildDataNodes(child_data_nodes);
        if (result != S_OK) {
            return result;
        }

        data_node = DataNode::CreateObject();
        for (const auto& each_child : child_data_nodes) {
            data_node->AddField(each_child.first, each_child.second);
        }

        return S_OK;
    }

    HRESULT ReadArray(std::shared_ptr<DataNode>& data_node) {

        std::vector<std::pair<std::wstring, std::shared_ptr<DataNode>>> child_data_nodes;
        HRESULT result = ReadChildDataNodes(child_data_nodes);
        if (result != S_OK) {
            return result;
        }

        data_node = DataNode::CreateArray();
        for (const auto& each_child : child_data_nodes) {
            data_node->AddElement(each_child.second);
        }

        return S_OK;
    }

    HRESULT ReadChildDataNodes(std::vector<std::pair<std::wstring, std::shared_ptr<DataNode>>>& child_data_nodes) {

        if (reader_->IsEmptyElement()) {
            return S_OK;
        }

        while (true) {

            XmlNodeType node_type;
            HRESULT result = AdvanceToNextNode(node_type);
            if (result != S_OK) {
                return result;
            }

            if (node_type == XmlNodeType_Element) {

                std::shared_ptr<DataNode> child_data_node;
                std::wstring key;
                HRESULT result = ReadDataNode(child_data_node, key);
                if (result != S_OK) {
                    return result;
                }

                child_data_nodes.emplace_back(key, child_data_node);
            }
            else if (node_type == XmlNodeType_EndElement) {
                return S_OK;
            }
            else {
                return E_INVALIDARG;
            }
        }
    }

    HRESULT ReadElementText(std::wstring& text) {

        if (reader_->IsEmptyElement()) {
            return S_OK;
        }

        XmlNodeType node_type;
        auto result = AdvanceToNextNode(node_type);
        if (result != S_OK) {
            return result;
        }

        if (node_type == XmlNodeType_Text) {

            const wchar_t* string = nullptr;
            result = reader_->GetValue(&string, nullptr);
            if (result != S_OK) {
                return result;
            }

            text = string;

            AdvanceToNextNode(node_type);
        }

        if (node_type == XmlNodeType_EndElement) {
            return S_OK;
        }

        return E_INVALIDARG;
    }

    HRESULT ReadKeyAttribute(std::wstring& key) {

        HRESULT result = S_OK;
        while (true) {

            result = reader_->MoveToNextAttribute();
            if (result != S_OK) {
                break;
            }

            const wchar_t* name = nullptr;
            result = reader_->GetLocalName(&name, nullptr);
            if (result != S_OK) {
                break;
            }

            if (std::wcscmp(name, internal::KeyAttributeName) == 0) {
            
                const wchar_t* value = nullptr;
                result = reader_->GetValue(&value, nullptr);
                if (result == S_OK) {
                    key = value;
                }
                break;
            }
        }

        return result;
    }

    HRESULT AdvanceToNextNode(XmlNodeType& next_node_type) {

        HRESULT result = S_OK;
        while (true) {

            result = reader_->Read(&next_node_type);
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

private:
    IXmlReader* reader_;
};

}


std::shared_ptr<DataNode> XmlReader::Read(const void* data, std::size_t data_length, std::error_code& error_code) {

    CComPtr<IXmlReader> xml_reader;
    HRESULT result = CreateXmlReader(__uuidof(IXmlReader), reinterpret_cast<void**>(&xml_reader), nullptr);
    if (FAILED(result)) {
        error_code = MakeComErrorCode(result);
        return nullptr;
    }

    auto stream = CreateMemoryStream(data, data_length);
    if (stream == nullptr) {
        error_code = MakeComErrorCode(E_OUTOFMEMORY);
        return nullptr;
    }

    result = xml_reader->SetInput(stream.GetHandle());
    if (FAILED(result)) {
        error_code = MakeComErrorCode(result);
        return nullptr;
    }

    DataNodeReader data_node_reader(xml_reader);
    std::shared_ptr<DataNode> data_node;
    result = data_node_reader.Execute(data_node);

    error_code = MakeComErrorCode(result);
    if (! IsSucceeded(error_code)) {
        return nullptr;
    }
    
    return data_node;
}

}