#include <zaf/crypto/md5.h>
#include <zaf/crypto/internal/crypto_manager.h>

namespace zaf::crypto {

MD5::MD5() : HashAlgorithm(internal::CryptoManager::Instance().MD5AlgorithmHandle()) {

}

}