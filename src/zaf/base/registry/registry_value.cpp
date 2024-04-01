#include <zaf/base/registry/registry_value.h>
#include <zaf/base/error/contract.h>

namespace zaf {

RegistryValue::RegistryValue(RegistryValueType type, std::vector<std::byte> buffer) :
    type_(type),
    buffer_(std::move(buffer)) {

}


RegistryValue::RegistryValue(RegistryValue&& other) :
    type_(other.type_),
    buffer_(std::move(other.buffer_)) {

    other.type_ = RegistryValueType::None;
}


RegistryValue& RegistryValue::operator=(RegistryValue&& other) {

    type_ = other.type_;
    buffer_ = std::move(other.buffer_);

    other.type_ = RegistryValueType::None;
    return *this;
}


std::wstring RegistryValue::ToString() const {

    ZAF_EXPECT(type_ == RegistryValueType::String);

    return InnerToString();
}


std::wstring RegistryValue::InnerToString() const {

    auto string = reinterpret_cast<const wchar_t*>(buffer_.data());
    auto string_length = buffer_.size() / sizeof(wchar_t);

    //The string buffer may or may not be terminated with \0.
    //Remove the last \0 if there is.
    if (string[string_length - 1] == L'\0') {
        --string_length;
    }

    return std::wstring{ string, string_length };
}


std::wstring RegistryValue::ToExpandableString() const {

    ZAF_EXPECT(type_ == RegistryValueType::ExpandableString);

    return InnerToString();
}


std::vector<std::wstring> RegistryValue::ToMultiString() const {

    ZAF_EXPECT(type_ == RegistryValueType::MultiString);

    std::vector<std::wstring> result;

    auto multi_string = reinterpret_cast<const wchar_t*>(buffer_.data());
    auto multi_string_length = buffer_.size() / sizeof(wchar_t);

    std::size_t string_begin_index{};
    std::size_t current_index{};
    while (current_index < multi_string_length) {

        if (multi_string[current_index] == L'\0') {

            auto string_length = current_index - string_begin_index;
            //Empty string is not allowed in multi-string,
            //ignore if it is.
            if (string_length > 0) {
                result.emplace_back(
                    multi_string + string_begin_index,
                    current_index - string_begin_index);
            }

            string_begin_index = current_index + 1;
        }

        ++current_index;
    }

    return result;
}


std::uint32_t RegistryValue::ToDWord() const {

    ZAF_EXPECT(type_ == RegistryValueType::DWord);

    auto result = *reinterpret_cast<const std::uint32_t*>(buffer_.data());
    return result;
}


std::uint64_t RegistryValue::ToQWord() const {

    ZAF_EXPECT(type_ == RegistryValueType::QWord);

    auto result = *reinterpret_cast<const std::uint64_t*>(buffer_.data());
    return result;
}

}