#pragma once

#include <mutex>
#include <optional>
#include <zaf/base/non_copyable.h>
#include <zaf/crypto/internal/algorithm_provider.h>

namespace zaf::crypto::internal {

class CryptoManager : NonCopyableNonMovable {
public:
    static CryptoManager& Instance();

    BCRYPT_ALG_HANDLE MD5AlgorithmProviderHandle();

private:
    std::optional<AlgorithmProvider> md5_algorithm_provider_;
    std::once_flag md5_algorithm_provider_once_flag_;
};

}