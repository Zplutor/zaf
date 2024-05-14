#pragma once

/**
@file
    Defines the zaf::ByteArray class.
*/

#include <cstddef>
#include <string>
#include <vector>

namespace zaf {

/**
An array container that stores bytes.

@details 
    ByteArray uses std::vector<std::byte> as the underlying storage, provides a similiar method set
    as it and extends more functionalities specific to a byte array.
*/
class ByteArray {
private:
    using InnerType = std::vector<std::byte>;

public:
    using iterator = InnerType::iterator;
    using const_iterator = InnerType::const_iterator;

public:
    /**
    Creates a ByteArray by copying content from the specified memory.

    @param data
        A pointer to the memory from which the array will be copied. The memory will be interpreted
        as a byte sequence.

    @param size
        The size of the bytes to be copied to the array.

    @pre
        data is not null.

    @return
        The new ByteArray whose content is identical to the specified memory.

    @throw std::bad_alloc
        Thrown if fails to allocate the required memory.
    */
    static ByteArray FromMemory(const void* data, std::size_t size);

    /**
    Creates a ByteArray by copying content from the specified string.

    @param string
        The string from which the array will be copied. The content of the string will be 
        interpreted as a byte sequence.

    @return
        The new ByteArray whose content is identical to the specified string, not including the 
        null terminator.

    @throw std::bad_alloc
        Thrown if fails to allocate the required memory.
    */
    static ByteArray FromString(std::string_view string);

    /**
    Creates a ByteArray by copying content from the specified wide string.

    @param string
        The wide string from which the array will be copied. The content of the string will be
        interpreted as a byte sequence.

    @return
        The new ByteArray whose content is identical to the specified string, not including the
        null terminator.

    @throw std::bad_alloc
        Thrown if fails to allocate the required memory.
    */
    static ByteArray FromString(std::wstring_view string);

public:
    /**
    Constructs an empty array.
    */
    ByteArray() noexcept = default;

    /**
    Constructs an array with bytes initialized to 0 and the specified size.

    @param size
        The size of the array.

    @throw std::bad_alloc
        Thrown if fails to allocate the required memory.
    */
    explicit ByteArray(std::size_t size);

    /**
    Constructs an array with the specified byte sequence.

    @param bytes
        The byte sequence assigned to the array.

    @throw std::bad_alloc
        Thrown if fails to allocate the required memory.
    */
    explicit ByteArray(std::initializer_list<std::byte> bytes);

    ByteArray(const ByteArray&) = default;
    ByteArray& operator=(const ByteArray&) = default;

    ByteArray(ByteArray&&) = default;
    ByteArray& operator=(ByteArray&&) = default;

    const std::byte* Data() const noexcept {
        return inner_.data();
    }

    std::byte* Data()  noexcept {
        return inner_.data();
    }

    std::size_t Size() const noexcept {
        return inner_.size();
    }

    /**
    Indicates whether the array is empty.
    */
    bool IsEmpty() const noexcept {
        return inner_.empty();
    }

    void Resize(std::size_t new_size) {
        inner_.resize(new_size);
    }

    /**
    Appends the specified byte to the end of the array.

    @param byte
        The byte to be appended.

    @throw std::bad_alloc
        Thrown if fails to allocate the required memory.
    */
    void Append(std::byte byte) {
        inner_.push_back(byte);
    }

    const std::byte& operator[](std::size_t index) const {
        return inner_[index];
    }

    std::byte& operator[](std::size_t index) {
        return inner_[index];
    }

    iterator begin()  noexcept {
        return inner_.begin();
    }

    iterator end() noexcept {
        return inner_.end();
    }

    const_iterator begin() const noexcept {
        return inner_.begin();
    }

    const_iterator end() const noexcept {
        return inner_.end();
    }

    const_iterator cbegin() const noexcept {
        return inner_.cbegin();
    }

    const_iterator cend() const noexcept {
        return inner_.cend();
    }

    friend auto operator<=>(const ByteArray&, const ByteArray&) noexcept = default;

    std::wstring ToHexString() const;

private:
    InnerType inner_;
};

}