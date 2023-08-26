#pragma once

#include <cstddef>
#include <vector>

namespace zaf {

class ByteArray {
private:
    using InnerType = std::vector<std::byte>;

public:
    using iterator = InnerType::iterator;
    using const_iterator = InnerType::const_iterator;

public:
    ByteArray() = default;
    explicit ByteArray(std::size_t size);

    ByteArray(const ByteArray&) = default;
    ByteArray& operator=(const ByteArray&) = default;

    ByteArray(ByteArray&&) = default;
    ByteArray& operator=(ByteArray&&) = default;

    const std::byte* Data() const {
        return inner_.data();
    }

    std::byte* Data() {
        return inner_.data();
    }

    std::size_t Size() const {
        return inner_.size();
    }

    void Resize(std::size_t new_size) {
        inner_.resize(new_size);
    }

    const std::byte& operator[](std::size_t index) const {
        return inner_[index];
    }

    std::byte& operator[](std::size_t index) {
        return inner_[index];
    }

    iterator begin() {
        return inner_.begin();
    }

    iterator end() {
        return inner_.end();
    }

    const_iterator begin() const {
        return inner_.begin();
    }

    const_iterator end() const {
        return inner_.end();
    }

    const_iterator cbegin() const {
        return inner_.cbegin();
    }

    const_iterator cend() const {
        return inner_.cend();
    }

    friend auto operator<=>(const ByteArray&, const ByteArray&) = default;

    std::wstring ToHexString() const;

private:
    InnerType inner_;
};

}