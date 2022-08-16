#include <zaf/base/registry/registry_value.h>
#include <zaf/base/error/check.h>

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

    std::wstring result{
        reinterpret_cast<const wchar_t*>(buffer_.data()),
        buffer_.size() / sizeof(wchar_t)
    };

    if (!result.empty()) {
        if (result.back() == L'\0') {
            result.pop_back();
        }
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