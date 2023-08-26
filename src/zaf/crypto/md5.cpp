#include <zaf/crypto/md5.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/crypto/internal/crypto_manager.h>

namespace zaf::crypto {

MD5::MD5() {

    NTSTATUS status = BCryptCreateHash(
        internal::CryptoManager::Instance().MD5AlgorithmProviderHandle(),
        &handle_,
        nullptr,
        0,
        nullptr,
        0,
        0);

    if (status != 0) {
        ZAF_THROW_ERRC(BasicErrc::Unknown);
    }
}


MD5::~MD5() {

    if (handle_) {
        BCryptDestroyHash(handle_);
    }
}


void MD5::Update(const std::byte* input, std::size_t size) {

    NTSTATUS status = BCryptHashData(
        handle_,
        reinterpret_cast<PUCHAR>(const_cast<std::byte*>(input)),
        static_cast<ULONG>(size),
        0);

    if (status != 0) {
        ZAF_THROW_ERRC(BasicErrc::Unknown);
    }
}


void MD5::Update(const char* input, std::size_t size) {
    Update(reinterpret_cast<const std::byte*>(input), size);
}


void MD5::Update(const unsigned char* input, std::size_t size) {
    Update(reinterpret_cast<const std::byte*>(input), size);
}


ByteArray MD5::Finish() {

    DWORD hash_length{};
    ULONG output_size{};
    NTSTATUS status = BCryptGetProperty(
        internal::CryptoManager::Instance().MD5AlgorithmProviderHandle(),
        BCRYPT_HASH_LENGTH,
        reinterpret_cast<PUCHAR>(&hash_length),
        sizeof(hash_length),
        &output_size,
        0);

    if (status != 0) {
        ZAF_THROW_ERRC(BasicErrc::Unknown);
    }

    ByteArray hash{ hash_length };

    status = BCryptFinishHash(handle_, reinterpret_cast<PUCHAR>(hash.Data()), hash_length, 0);
    if (status != 0) {
        ZAF_THROW_ERRC(BasicErrc::Unknown);
    }

    return hash;
}

}