#include <zaf/resource/internal/resource_loader.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/container/utility/map.h>
#include <zaf/base/error/system_error.h>

namespace zaf::internal {

ResoureLoader::~ResoureLoader() {

    for (auto each_module : Values(dll_modules_)) {
        FreeLibrary(each_module);
    }
}


Stream ResoureLoader::Load(const std::wstring& dll, const std::wstring& name) {

    HMODULE module{};
    if (!dll.empty()) {
        module = GetDLLModule(dll);
    }

    HRSRC resource_handle = FindResource(module, name.c_str(), L"ZRC");
    if (!resource_handle) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    HGLOBAL memory_handle = LoadResource(module, resource_handle);
    if (!memory_handle) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    DWORD resource_size = SizeofResource(module, resource_handle);
    if (resource_size <= 0) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    LPVOID resource_data = LockResource(memory_handle);
    if (!resource_data) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    return Stream::FromMemoryNotOwn(resource_data, resource_size);
}


HMODULE ResoureLoader::GetDLLModule(const std::wstring& dll) {

    {
        std::lock_guard<std::mutex> lock_guard{ lock_ };

        auto module = Find(dll_modules_, dll);
        if (module) {
            return *module;
        }
    }

    HMODULE module = LoadLibrary(dll.c_str());
    if (!module) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    {
        std::lock_guard<std::mutex> lock_guard{ lock_ };

        auto insert_result = dll_modules_.insert(std::make_pair(dll, module));
        if (!insert_result.second) {
            FreeLibrary(module);
        }
        return insert_result.first->second;
    }
}

}