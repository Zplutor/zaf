#pragma once

#include <mutex>
#include <optional>
#include <zaf/base/non_copyable.h>
#include <zaf/crypto/internal/algorithm_provider.h>

namespace zaf::crypto::internal {

class CryptoManager : NonCopyableNonMovable {
public:
    static CryptoManager& Instance();

    BCRYPT_ALG_HANDLE MD5AlgorithmHandle() {
        return md5_algorithm_.Handle();
    }

    BCRYPT_ALG_HANDLE SHA1AlgorithmHandle() {
        return sha1_algorithm_.Handle();
    }

    BCRYPT_ALG_HANDLE SHA256AlgorithmHandle() {
        return sha256_algorithm_.Handle();
    }

    BCRYPT_ALG_HANDLE SHA384AlgorithmHandle() {
        return sha384_algorithm_.Handle();
    }

    BCRYPT_ALG_HANDLE SHA512AlgorithmHandle() {
        return sha512_algorithm_.Handle();
    }

private:
    AlgorithmProvider md5_algorithm_{ BCRYPT_MD5_ALGORITHM };
    AlgorithmProvider sha1_algorithm_{ BCRYPT_SHA1_ALGORITHM };
    AlgorithmProvider sha256_algorithm_{ BCRYPT_SHA256_ALGORITHM };
    AlgorithmProvider sha384_algorithm_{ BCRYPT_SHA384_ALGORITHM };
    AlgorithmProvider sha512_algorithm_{ BCRYPT_SHA512_ALGORITHM };
};

}