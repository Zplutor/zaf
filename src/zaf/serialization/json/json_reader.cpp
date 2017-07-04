#include <zaf/serialization/json/json_reader.h>
#include <cwctype>
#include <zaf/base/string/encoding_conversion.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/serialization/data_node.h>

namespace zaf {
namespace {

enum class TokenType {
    None,
    WhiteSpace,
    String,
    Text,
    ObjectBegin,
    ObjectEnd,
    FieldIdentifier,
    ArrayBegin,
    ArrayEnd,
    Delimiter,
};

class Parser {
public:
    Parser(const std::wstring& string) : string_(string) {
        next_cursor_ = string_.begin();
        end_cursor_ = string_.end();
    }

    TokenType GetTokenType() {
        return token_type_;
    }

    const std::wstring& GetValue() {
        return value_;
    }

    bool Advance(TokenType& token_type) {

        token_type_ = TokenType::None;
        value_.clear();

        if (IsEnd()) {
            return true;
        }

        bool is_succeeded = false;
        auto current_char = ReadChar();
        switch (current_char) {

        case L'"':
            is_succeeded = ParseString();
            break;

        case L'{':
            token_type_ = TokenType::ObjectBegin;
            is_succeeded = true;
            break;

        case L'}':
            token_type_ = TokenType::ObjectEnd;
            is_succeeded = true;
            break;

        case L':':
            token_type_ = TokenType::FieldIdentifier;
            is_succeeded = true;
            break;

        case L'[':
            token_type_ = TokenType::ArrayBegin;
            is_succeeded = true;
            break;

        case L']':
            token_type_ = TokenType::ArrayEnd;
            is_succeeded = true;
            break;

        case L',':
            token_type_ = TokenType::Delimiter;
            is_succeeded = true;
            break;

        default:
            if (std::iswspace(current_char)) {
                is_succeeded = ParseWhiteSpace();
            }
            else {
                is_succeeded = ParseText();
            }
            break;
        }

        token_type = token_type_;
        return true;
    }

private:
    bool ParseString() {

        std::wstring string;

        while (! IsEnd()) {

            auto ch = ReadChar();
            if (ch == L'"') {
                break;
            }
            else {
                string.append(1, ch);
            }
        }

        if (GetChar() != L'"') {
            return false;
        }

        token_type_ = TokenType::String;
        value_ = std::move(string);
        return true;
    }

    bool ParseText() {

        std::wstring text(1, GetChar());

        while (! IsEnd()) {

            bool should_end = false;

            auto current_char = ReadChar();
            switch (current_char) {

            case L'"':
            case L'{':
            case L'}':
            case L'[':
            case L']':
            case L':':
            case L',':
                should_end = true;
                break;
            default:
                should_end = std::iswspace(current_char) != 0;
                break;
            }

            if (should_end) {
                Back();
                break;
            }
            
            text.append(1, current_char);
        }
        
        token_type_ = TokenType::Text;
        value_ = std::move(text);
        return true;
    }

    bool ParseWhiteSpace() {

        while (! IsEnd()) {
            if (!std::iswspace(ReadChar())) {
                Back();
                break;
            }
        }

        token_type_ = TokenType::WhiteSpace;
        return true;
    }

    wchar_t ReadChar() {
        current_char_ = *next_cursor_;
        ++next_cursor_;
        return current_char_;
    }

    wchar_t GetChar() {
        return current_char_;
    }

    void Back() {
        --next_cursor_;
    }

    bool IsEnd() const {
        return next_cursor_ == end_cursor_;
    }

private:
    std::wstring string_;
    wchar_t current_char_ = 0;
    std::wstring::const_iterator next_cursor_;
    std::wstring::const_iterator end_cursor_;

    TokenType token_type_ = TokenType::None;
    std::wstring value_;
};


class JsonDataNodeReader {
public:
    JsonDataNodeReader(const std::wstring& json_string) : parser_(json_string) {
        
    }

    bool Read(std::shared_ptr<DataNode>& data_node) {

        TokenType token_type;
        if (! SkipWhiteSpaces(token_type)) {
            return false;
        }

        return ReadDataNode(data_node);
    }

private:
    bool ReadDataNode(std::shared_ptr<DataNode>& data_node) {

        switch (parser_.GetTokenType()) {
        case TokenType::String:
            return ReadString(data_node);

        case TokenType::Text:
            return ReadBooleanOrNumber(data_node);

        case TokenType::ObjectBegin:
            return ReadObject(data_node);

        case TokenType::ArrayBegin:
            return ReadArray(data_node);

        default:
            return false;
        }
    }

    bool ReadString(std::shared_ptr<DataNode>& data_node) {
        data_node = DataNode::CreateString(parser_.GetValue());
        return true;
    }

    bool ReadBooleanOrNumber(std::shared_ptr<DataNode>& data_node) {

        const auto& value = parser_.GetValue();
        if (value == L"true") {
            data_node = DataNode::CreateBoolean(true);
            return true;
        }

        if (value == L"false") {
            data_node = DataNode::CreateBoolean(false);
            return true;
        }
        
        std::int64_t signed_integer;
        if (zaf::TryToNumeric(value, signed_integer)) {
            data_node = DataNode::CreateNumber(signed_integer);
            return true;
        }

        std::uint64_t unsigned_integer;
        if (zaf::TryToNumeric(value, unsigned_integer)) {
            data_node = DataNode::CreateNumber(unsigned_integer);
            return true;
        }

        double floating_point;
        if (zaf::TryToNumeric(value, floating_point)) {
            data_node = DataNode::CreateNumber(floating_point);
            return true;
        }

        return false;
    }

    bool ReadObject(std::shared_ptr<DataNode>& data_node) {

        data_node = DataNode::CreateObject();

        TokenType token_type;
        bool is_first_field = true;
        while (true) {

            if (! SkipWhiteSpaces(token_type)) {
                return false;
            }

            if (token_type == TokenType::Delimiter) {

                if (is_first_field) {
                    return false;
                }

                if (!SkipWhiteSpaces(token_type)) {
                    return false;
                }
            }

            if (token_type != TokenType::String) {
                break;
            }

            auto key = parser_.GetValue();

            if (! SkipWhiteSpaces(token_type)) {
                return false;
            }

            if (token_type != TokenType::FieldIdentifier) {
                return false;
            }

            if (! SkipWhiteSpaces(token_type)) {
                return false;
            }

            std::shared_ptr<DataNode> field_data_node;
            if (! ReadDataNode(field_data_node)) {
                return false;
            }

            data_node->AddChild(key, field_data_node);
            is_first_field = false;
        }

        return token_type == TokenType::ObjectEnd;
    }

    bool ReadArray(std::shared_ptr<DataNode>& data_node) {

        data_node = DataNode::CreateArray();

        TokenType token_type;
        bool is_first_element = true;
        while (true) {

            if (! SkipWhiteSpaces(token_type)) {
                return false;
            }

            if (token_type == TokenType::Delimiter) {

                if (is_first_element) {
                    return false;
                }

                if (!SkipWhiteSpaces(token_type)) {
                    return false;
                }
            }

            if (token_type == TokenType::ArrayEnd) {
                break;
            }

            std::shared_ptr<DataNode> element_data_node;
            if (! ReadDataNode(element_data_node)) {
                return false;
            }

            data_node->AddChild(element_data_node);
            is_first_element = false;
        }

        return token_type == TokenType::ArrayEnd;
    }

    bool SkipWhiteSpaces(TokenType& token_type) {

        while (true) {
            
            if (! parser_.Advance(token_type)) {
                return false;
            }

            if (token_type != TokenType::WhiteSpace) {
                break;
            }
        }

        return true;
    }

private:
    Parser parser_;
    std::shared_ptr<DataNode> data_node_;
};

}

std::shared_ptr<DataNode> JsonReader::Read(const void* data, std::size_t data_length, std::error_code& error_code) {

    auto json_string = FromUtf8String(reinterpret_cast<const char*>(data), data_length);

    JsonDataNodeReader reader(json_string);
    std::shared_ptr<DataNode> data_node;
    if (! reader.Read(data_node)) {
        error_code = std::make_error_code(std::errc::invalid_argument);
        return nullptr;
    }

    error_code.clear();
    return data_node;
}

}