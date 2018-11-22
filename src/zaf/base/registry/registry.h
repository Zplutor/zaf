#pragma once

#include "zaf/base/registry/registry_key.h"

namespace zaf {

class Registry {
public:
    static RegistryKey ClassesRoot() {
        return RegistryKey(HKEY_CLASSES_ROOT);
    }

    static RegistryKey CurrentUser() {
        return RegistryKey(HKEY_CURRENT_USER);
    }

    static RegistryKey LocalMachine() {
        return RegistryKey(HKEY_LOCAL_MACHINE);
    }

    static RegistryKey Users() {
        return RegistryKey(HKEY_USERS);
    }

    static RegistryKey CurrentConfig() {
        return RegistryKey(HKEY_CURRENT_CONFIG);
    }
   
public:
    ~Registry() = delete;
};

}