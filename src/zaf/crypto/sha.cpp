#include <zaf/crypto/sha.h>
#include <zaf/crypto/internal/crypto_manager.h>

namespace zaf::crypto {

SHA1::SHA1() : HashAlgorithm(internal::CryptoManager::Instance().SHA1AlgorithmHandle()) {

}


SHA256::SHA256() : HashAlgorithm(internal::CryptoManager::Instance().SHA256AlgorithmHandle()) {

}


SHA384::SHA384() : HashAlgorithm(internal::CryptoManager::Instance().SHA384AlgorithmHandle()) {

}


SHA512::SHA512() : HashAlgorithm(internal::CryptoManager::Instance().SHA512AlgorithmHandle()) {

}

}