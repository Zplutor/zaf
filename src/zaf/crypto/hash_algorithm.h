#pragma once

#include <zaf/base/byte_array.h>
#include <zaf/base/non_copyable.h>

namespace zaf::crypto {

class HashAlgorithm : NonCopyable {
public:
    explicit HashAlgorithm(void* algorithm_handle);
    ~HashAlgorithm();

    HashAlgorithm(HashAlgorithm&& other) noexcept;
    HashAlgorithm& operator=(HashAlgorithm&& other) noexcept;

    void Update(const std::byte* input, std::size_t size);
    void Update(const char* input, std::size_t size);
    void Update(const unsigned char* input, std::size_t size);

    ByteArray Finish();

    void* Handle() const {
        return hash_handle_;
    }

private:
    void Reset();
    void MoveFromOther(HashAlgorithm& other);

private:
    void* algorithm_handle_{};
    void* hash_handle_{};
};

}