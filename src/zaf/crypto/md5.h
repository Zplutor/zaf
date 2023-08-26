#pragma once

#include <zaf/crypto/hash_algorithm.h>

namespace zaf::crypto {

class MD5 : public HashAlgorithm {
public:
    MD5();
};

}