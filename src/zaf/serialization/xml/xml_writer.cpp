#include <zaf/serialization/xml/xml_writer.h>
#include <atlbase.h>
#include <xmllite.h>
#include <zaf/base/stream.h>
#include <zaf/base/string/to_string.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/xml/internal/xml_names.h>

namespace zaf {
namespace {

class DataNodeWriter {
public:
    DataNodeWriter(const DataNode& data_node, IXmlWriter* writer) : 
        data_node_(data_node),
        writer_(writer) {
        
    }

    HRESULT Execute() {
        return WriteDataNode(data_node_, std::wstring());
    }

private:
    HRESULT WriteDataNode(const DataNode& data_node, const std::wstring& key) {

        if (data_node.IsObject()) {
            return WriteObject(data_node, key);
        }
        else if (data_node.IsArray()) {
            return WriteArray(data_node, key);
        }
        else if (data_node.IsString()) {
            return WriteString(data_node, key);
        }
        else if (data_node.IsBoolean()) {
            return WriteBoolean(data_node, key);
        }
        else if (data_node.IsNumber()) {
            return WriteNumber(data_node, key);
        }
        else {
            return E_NOTIMPL;
        }
    }


    HRESULT WriteObject(const DataNode& data_object, const std::wstring& key) {

        HRESULT result = writer_->WriteStartElement(nullptr, internal::ObjectElementName, nullptr);
        if (FAILED(result)) {
            return result;
        }

        result = WriteKeyAttribute(key);
        if (FAILED(result)) {
            return result;
        }

        data_object.EnumerateKeyedChildren([this, &result](const std::wstring& key, const DataNode& data_node) {

            if (FAILED(result)) {
                return;
            }

            result = WriteDataNode(data_node, key);
        });

        if (FAILED(result)) {
            return result;
        }
        
        return writer_->WriteEndElement();
    }

    HRESULT WriteArray(const DataNode& data_node, const std::wstring& key) {

        HRESULT result = writer_->WriteStartElement(nullptr, internal::ArrayElementName, nullptr);
        if (FAILED(result)) {
            return result;
        }

        result = WriteKeyAttribute(key);
        if (FAILED(result)) {
            return result;
        }

        data_node.EnumerateChildren([this, &result](const DataNode& data_node) {
        
            if (FAILED(result)) {
                return;
            }

            result = WriteDataNode(data_node, std::wstring());
        });

        if (FAILED(result)) {
            return result;
        }

        return writer_->WriteEndElement();
    }

    HRESULT WriteString(const DataNode& data_node, const std::wstring& key) {
        return WriteTextElement(internal::StringElementName, key, data_node.GetString());
    }

    HRESULT WriteBoolean(const DataNode& data_node, const std::wstring& key) {
        const wchar_t* const value = data_node.GetBoolean() ? L"true" : L"false";
        return WriteTextElement(internal::BooleanElementName, key, value);
    }

    HRESULT WriteNumber(const DataNode& data_node, const std::wstring& key) {

        auto number = data_node.GetNumber();

        std::wstring string;
        auto tag = number.GetTag();
        switch (tag) {
        case Number::Tag::SignedInteger:
            string = zaf::ToWideString(number.GetValue<std::int64_t>());
            break;
        case Number::Tag::UnsignedInteger:
            string = zaf::ToWideString(number.GetValue<std::uint64_t>());
            break;
        case Number::Tag::FloatingPoint:
            string = zaf::ToWideString(number.GetValue<double>());
            break;
        default:
            return E_INVALIDARG;
        }

        return WriteTextElement(internal::NumberElementName, key, string);
    }

    HRESULT WriteTextElement(const std::wstring& element_name, const std::wstring& key, const std::wstring& text) {

        HRESULT result = writer_->WriteStartElement(nullptr, element_name.c_str(), nullptr);
        if (FAILED(result)) {
            return result;
        }

        result = WriteKeyAttribute(key);
        if (FAILED(result)) {
            return result;
        }

        result = writer_->WriteString(text.c_str());
        if (FAILED(result)) {
            return result;
        }

        return writer_->WriteEndElement();
    }

    HRESULT WriteKeyAttribute(const std::wstring& key) {

        if (key.empty()) {
            return S_OK;
        }

        return writer_->WriteAttributeString(nullptr, internal::KeyAttributeName, nullptr, key.c_str());
    }

private:
    const DataNode& data_node_;
    IXmlWriter* writer_;
};


std::string GetStringFromStream(Stream& stream, std::error_code& error_code) {

    stream.Seek(Stream::Origin::Set, 0, error_code);
    if (! IsSucceeded(error_code)) {
        return std::string();
    }

    auto length = stream.GetLength(error_code);
    if (!IsSucceeded(error_code)) {
        return std::string();
    }

    std::string string(static_cast<std::size_t>(length), 0);
    stream.Read(static_cast<std::size_t>(length), &string[0], error_code);
    if (!IsSucceeded(error_code)) {
        return std::string();
    }

    return string;
}

}


std::string XmlWriter::Write(const std::shared_ptr<DataNode>& data_node, std::error_code& error_code) {

    CComPtr<IXmlWriter> xml_writer;
    HRESULT result = CreateXmlWriter(__uuidof(IXmlWriter), reinterpret_cast<void**>(&xml_writer), nullptr); 
    if (FAILED(result)) {
        error_code = MakeComErrorCode(result);
        return std::string();
    }

    auto stream = CreateMemoryStream();
    if (stream == nullptr) {
        error_code = MakeComErrorCode(E_OUTOFMEMORY);
        return std::string();
    }

    result = xml_writer->SetOutput(stream.GetHandle());
    if (FAILED(result)) {
        error_code = MakeComErrorCode(result);
        return std::string();
    }

    DataNodeWriter data_node_writer(*data_node, xml_writer);
    result = data_node_writer.Execute();
    if (FAILED(result)) {
        error_code = MakeComErrorCode(result);
        return std::string();
    }

    result = xml_writer->Flush();
    if (FAILED(result)) {
        return std::string();
    }

    return GetStringFromStream(stream, error_code);
}

}

