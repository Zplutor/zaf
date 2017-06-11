#include <zaf/serialization/xml/xml_writer.h>
#include <atlbase.h>
#include <xmllite.h>
#include <zaf/base/string/to_string.h>
#include <zaf/serialization/data_node.h>

namespace zaf {
namespace {

const wchar_t* const ArrayElementName = L"array";
const wchar_t* const BooleanElementName = L"boolean";
const wchar_t* const FieldElementName = L"field";
const wchar_t* const KeyAttributeName = L"key";
const wchar_t* const NumberElementName = L"number";
const wchar_t* const ObjectElementName = L"object";
const wchar_t* const StringElementName = L"string";

class DataNodeWriter {
public:
    DataNodeWriter(const DataNode& data_node, IXmlWriter* writer) : 
        data_node_(data_node),
        writer_(writer) {
        
    }

    HRESULT Execute() {
        return WriteDataNode(data_node_);
    }

private:
    HRESULT WriteDataNode(const DataNode& data_node) {

        if (data_node.IsObject()) {
            return WriteObject(data_node);
        }
        else if (data_node.IsArray()) {
            return WriteArray(data_node);
        }
        else if (data_node.IsString()) {
            return WriteString(data_node);
        }
        else if (data_node.IsBoolean()) {
            return WriteBoolean(data_node);
        }
        else if (data_node.IsNumber()) {
            return WriteNumber(data_node);
        }
        else {
            return E_NOTIMPL;
        }
    }


    HRESULT WriteObject(const DataNode& data_object) {

        HRESULT result = writer_->WriteStartElement(nullptr, ObjectElementName, nullptr);
        if (FAILED(result)) {
            return result;
        }

        data_object.EnumerateFields([this, &result](const std::wstring& key, const DataNode& data_node) {

            if (FAILED(result)) {
                return;
            }

            result = WriteField(key, data_node);
        });

        if (FAILED(result)) {
            return result;
        }
        
        return writer_->WriteEndElement();
    }

    HRESULT WriteField(const std::wstring& key, const DataNode& data_node) {

        HRESULT result = writer_->WriteStartElement(nullptr, FieldElementName, nullptr);
        if (FAILED(result)) {
            return result;
        }

        result = writer_->WriteAttributeString(nullptr, KeyAttributeName, nullptr, key.c_str());
        if (FAILED(result)) {
            return result;
        }

        result = WriteDataNode(data_node);
        if (FAILED(result)) {
            return result;
        }

        return writer_->WriteEndElement();
    }

    HRESULT WriteArray(const DataNode& data_node) {

        HRESULT result = writer_->WriteStartElement(nullptr, ArrayElementName, nullptr);
        if (FAILED(result)) {
            return result;
        }

        data_node.EnumerateElements([this, &result](const DataNode& data_node) {
        
            if (FAILED(result)) {
                return;
            }

            result = WriteDataNode(data_node);
        });

        if (FAILED(result)) {
            return result;
        }

        return writer_->WriteEndElement();
    }

    HRESULT WriteString(const DataNode& data_node) {
        return writer_->WriteElementString(nullptr, StringElementName, nullptr, data_node.GetString().c_str());
    }

    HRESULT WriteBoolean(const DataNode& data_node) {
        const wchar_t* const value = data_node.GetBoolean() ? L"true" : L"false";
        return writer_->WriteElementString(nullptr, BooleanElementName, nullptr, value);
    }

    HRESULT WriteNumber(const DataNode& data_node) {

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

        return writer_->WriteElementString(nullptr, NumberElementName, nullptr, string.c_str());
    }

private:
    const DataNode& data_node_;
    IXmlWriter* writer_;
};

}

void XmlWriter::Execute(
    const std::shared_ptr<DataNode>& data_node, 
    const Stream& output_stream,
    std::error_code& error_code) {

    CComPtr<IXmlWriter> xml_writer;
    HRESULT result = CreateXmlWriter(__uuidof(IXmlWriter), reinterpret_cast<void**>(&xml_writer), nullptr); 
    if (FAILED(result)) {
        error_code = MakeComErrorCode(result);
        return;
    }

    result = xml_writer->SetOutput(output_stream.GetHandle());
    if (FAILED(result)) {
        error_code = MakeComErrorCode(result);
        return;
    }

    DataNodeWriter data_node_writer(*data_node, xml_writer);
    result = data_node_writer.Execute();
    error_code = MakeComErrorCode(result);
}

}

