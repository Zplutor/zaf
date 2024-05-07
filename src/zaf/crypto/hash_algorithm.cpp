#include <zaf/crypto/hash_algorithm.h>
#include <zaf/base/error/nt_error.h>

namespace zaf::crypto {

HashAlgorithm::HashAlgorithm(void* algorithm_handle) : algorithm_handle_(algorithm_handle) {

    NTSTATUS status = BCryptCreateHash(
        algorithm_handle_,
        &hash_handle_,
        nullptr,
        0,
        nullptr,
        0,
        0);

    if (!BCRYPT_SUCCESS(status)) {
        throw NTError{ status, ZAF_SOURCE_LOCATION() };
    }
}


HashAlgorithm::~HashAlgorithm() {
    Reset();
}


HashAlgorithm::HashAlgorithm(HashAlgorithm&& other) noexcept {
    MoveFromOther(other);
}


HashAlgorithm& HashAlgorithm::operator=(HashAlgorithm&& other) noexcept {
    Reset();
    MoveFromOther(other);
    return *this;
}


void HashAlgorithm::Reset() {

    if (hash_handle_) {
        BCryptDestroyHash(hash_handle_);
        hash_handle_ = nullptr;
    }
}


void HashAlgorithm::MoveFromOther(HashAlgorithm& other) {

    algorithm_handle_ = other.algorithm_handle_;
    hash_handle_ = other.hash_handle_;
    
    other.hash_handle_ = nullptr;
}


void HashAlgorithm::Update(const std::byte* input, std::size_t size) {

    NTSTATUS status = BCryptHashData(
        hash_handle_,
        reinterpret_cast<PUCHAR>(const_cast<std::byte*>(input)),
        static_cast<ULONG>(size),
        0);

    if (!BCRYPT_SUCCESS(status)) {
        throw NTError{ status, ZAF_SOURCE_LOCATION() };
    }
}


void HashAlgorithm::Update(const char* input, std::size_t size) {
    Update(reinterpret_cast<const std::byte*>(input), size);
}


void HashAlgorithm::Update(const unsigned char* input, std::size_t size) {
    Update(reinterpret_cast<const std::byte*>(input), size);
}


ByteArray HashAlgorithm::Finish() {

    DWORD hash_length{};
    ULONG output_size{};
    NTSTATUS status = BCryptGetProperty(
        algorithm_handle_,
        BCRYPT_HASH_LENGTH,
        reinterpret_cast<PUCHAR>(&hash_length),
        sizeof(hash_length),
        &output_size,
        0);

    if (!BCRYPT_SUCCESS(status)) {
        throw NTError{ status, ZAF_SOURCE_LOCATION() };
    }

    ByteArray hash{ hash_length };

    status = BCryptFinishHash(hash_handle_, reinterpret_cast<PUCHAR>(hash.Data()), hash_length, 0);
    if (!BCRYPT_SUCCESS(status)) {
        throw NTError{ status, ZAF_SOURCE_LOCATION() };
    }

    return hash;
}

}