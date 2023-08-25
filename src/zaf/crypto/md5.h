#pragma once

#include <Windows.h>
#include <bcrypt.h>
#include <zaf/base/non_copyable.h>

namespace zaf::crypto {

class MD5 : NonCopyable {
public:
    MD5();
    ~MD5();

    void Update(const std::byte* input, std::size_t size);
    void Update(const char* input, std::size_t size);
    void Update(const unsigned char* input, std::size_t size);

    std::vector<std::byte> Finish();

private:
    BCRYPT_HASH_HANDLE handle_{};
};

}