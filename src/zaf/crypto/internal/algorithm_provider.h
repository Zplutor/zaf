#pragma once

#include <bcrypt.h>
#include <mutex>
#include <zaf/base/error/nt_error.h>
#include <zaf/base/non_copyable.h>

namespace zaf::crypto::internal {

class AlgorithmProvider : NonCopyableNonMovable {
public:
    explicit AlgorithmProvider(const wchar_t* algorithm_id) : algorithm_id_(algorithm_id) {
    
    }

    ~AlgorithmProvider() {
        if (handle_) {
            BCryptCloseAlgorithmProvider(handle_, 0);
        }
    }

    BCRYPT_ALG_HANDLE Handle() {

        std::call_once(once_flag_, [this]() {
        
            NTSTATUS status = BCryptOpenAlgorithmProvider(
                &handle_,
                algorithm_id_,
                nullptr,
                0);

            if (!BCRYPT_SUCCESS(status)) {
                throw NTError{ status, ZAF_SOURCE_LOCATION() };
            }
        });
        return handle_;
    }

private:
    const wchar_t* algorithm_id_{};
    BCRYPT_ALG_HANDLE handle_{};
    std::once_flag once_flag_;
};

}