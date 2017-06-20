#include <zaf/serialization/json/json_writer.h>
#include <zaf/base/string/encoding_conversion.h>
#include <zaf/serialization/data_node.h>

namespace zaf {
namespace {

class DataNodeWriter {
public:
    const std::wstring& GetJsonString() const {
        return json_string_;
    }

    HRESULT WriteDataNode(const DataNode& data_node) {

        switch (data_node.GetType()) {

        case DataNode::Type::String:
            return WriteString(data_node);

        case DataNode::Type::Number:
            return WriteNumber(data_node);

        case DataNode::Type::Boolean:
            return WriteBoolean(data_node);

        case DataNode::Type::Object:
            return WriteObject(data_node);

        case DataNode::Type::Array:
            return WriteArray(data_node);

        default:
            return E_INVALIDARG;
        }
    }

private:
    HRESULT WriteString(const DataNode& data_node) {
        WriteString(data_node.GetString());
        return S_OK;
    }

    HRESULT WriteBoolean(const DataNode& data_node) {
        json_string_.append(data_node.GetBoolean() ? L"true" : L"false");
        return S_OK;
    }

    HRESULT WriteNumber(const DataNode& data_node) {

        std::wstring string;

        auto number = data_node.GetNumber();
        switch (number.GetTag()) {
        case Number::Tag::SignedInteger:
            string = std::to_wstring(number.GetValue<std::int64_t>());
            break;
        case Number::Tag::UnsignedInteger:
            string = std::to_wstring(number.GetValue<std::uint64_t>());
            break;
        case Number::Tag::FloatingPoint:
            string = std::to_wstring(number.GetValue<double>());
            break;
        }

        json_string_.append(string);
        return S_OK;
    }

    HRESULT WriteObject(const DataNode& data_node) {

        json_string_.append(1, L'{');
        
        bool is_first = true;
        HRESULT result = S_OK;
        data_node.EnumerateKeyedChildren([this, &is_first, &result](const std::wstring& key, const DataNode& field_node) {
        
            if (FAILED(result)) {
                return;
            }

            if (! is_first) {
                json_string_.append(1, L',');
            }
            else {
                is_first = false;
            }

            WriteString(key);
            json_string_.append(1, L':');

            result = WriteDataNode(field_node);
        });

        if (FAILED(result)) {
            return result;
        }

        json_string_.append(1, L'}');
        return S_OK;
    }

    HRESULT WriteArray(const DataNode& data_node) {

        json_string_.append(1, L'[');

        bool is_first = true;
        HRESULT result = S_OK;
        data_node.EnumerateChildren([this, &is_first, &result](const DataNode& element_node) {
        
            if (FAILED(result)) {
                return;
            }

            if (! is_first) {
                json_string_.append(1, L',');
            }
            else {
                is_first = false;
            }

            result = WriteDataNode(element_node);
        });

        if (FAILED(result)) {
            return result;
        }

        json_string_.append(1, L']');
        return S_OK;
    }

    void WriteString(const std::wstring& string) {
        json_string_.append(1, L'"');
        json_string_.append(string);
        json_string_.append(1, L'"');
    }

private:
    std::wstring json_string_;
};

}

std::string JsonWriter::Write(const std::shared_ptr<DataNode>& data_node, std::error_code& error_code) {

    DataNodeWriter writer;
    auto result = writer.WriteDataNode(*data_node);
    if (FAILED(result)) {
        error_code = MakeComErrorCode(result);
        return std::string();
    }
    
    return ToUtf8String(writer.GetJsonString());
}

}