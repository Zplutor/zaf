#include <zaf/crypto/internal/crypto_manager.h>
#include <zaf/application.h>
#include <zaf/base/error/basic_error.h>

namespace zaf::crypto::internal {

CryptoManager& CryptoManager::Instance() {
    return Application::Instance().CryptoManager();
}


BCRYPT_ALG_HANDLE CryptoManager::MD5AlgorithmProviderHandle() {

    std::call_once(md5_algorithm_provider_once_flag_, [this]() {

        BCRYPT_ALG_HANDLE handle{};
        NTSTATUS status = BCryptOpenAlgorithmProvider(&handle, BCRYPT_MD5_ALGORITHM, nullptr, 0);
        if (status != 0) {
            ZAF_THROW_ERRC(BasicErrc::Unknown);
        }

        md5_algorithm_provider_.emplace(handle);
    });

    return md5_algorithm_provider_->Handle();
}

}