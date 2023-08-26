#pragma once

#include <zaf/crypto/hash_algorithm.h>

namespace zaf::crypto {

class SHA1 : public HashAlgorithm {
public:
    SHA1();
};


class SHA256 : public HashAlgorithm {
public:
    SHA256();
};


class SHA384 : public HashAlgorithm {
public:
    SHA384();
};


class SHA512 : public HashAlgorithm {
public:
    SHA512();
};

}