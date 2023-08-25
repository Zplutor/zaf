#pragma once

#include <bcrypt.h>
#include <zaf/base/non_copyable.h>

namespace zaf::crypto::internal {

class AlgorithmProvider : NonCopyableNonMovable {
public:
    explicit AlgorithmProvider(BCRYPT_ALG_HANDLE handle) : handle_(handle) { }

    ~AlgorithmProvider() {
        if (handle_) {
            BCryptCloseAlgorithmProvider(handle_, 0);
        }
    }

    BCRYPT_ALG_HANDLE Handle() const {
        return handle_;
    }

private:
    BCRYPT_ALG_HANDLE handle_{};
};

}