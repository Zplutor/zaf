#include <zaf/crypto/internal/crypto_manager.h>
#include <zaf/application.h>

namespace zaf::crypto::internal {

CryptoManager& CryptoManager::Instance() {
    return Application::Instance().CryptoManager();
}

}