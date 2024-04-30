#include <zaf/resource/internal/resource_loader.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/container/utility/map.h>
#include <zaf/base/error/win32_error.h>
#include <zaf/base/string/case_conversion.h>

namespace zaf::internal {
namespace {

constexpr int SupportedScalePercents[]{
    100,
    125,
    150,
    175,
    200,
};

}

ResourceLoader::~ResourceLoader() {

    for (auto each_module : Values(dll_modules_)) {
        FreeLibrary(each_module);
    }
}


Stream ResourceLoader::Load(const std::wstring& dll, const std::wstring& name, float dpi) {

    HMODULE module{};
    if (!dll.empty()) {
        module = GetDLLModule(dll);
    }

    HRSRC resource_handle = FindResouceMatchesDPI(module, name, dpi);

    HGLOBAL memory_handle = LoadResource(module, resource_handle);
    if (!memory_handle) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    DWORD resource_size = SizeofResource(module, resource_handle);
    if (resource_size <= 0) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    LPVOID resource_data = LockResource(memory_handle);
    if (!resource_data) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    return Stream::FromMemoryNoCopy(resource_data, resource_size);
}


HMODULE ResourceLoader::GetDLLModule(const std::wstring& dll) {

    {
        std::lock_guard<std::mutex> lock_guard{ lock_ };

        auto module = Find(dll_modules_, dll);
        if (module) {
            return *module;
        }
    }

    HMODULE module = LoadLibrary(dll.c_str());
    if (!module) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
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


HRSRC ResourceLoader::FindResouceMatchesDPI(HMODULE module, const std::wstring& name, float dpi) {

    constexpr const wchar_t* zaf_resource_type_name = L"ZRC";

    if (!IsImageResource(name)) {

        auto result = FindResource(module, name.c_str(), zaf_resource_type_name);
        if (!result) {
            ZAF_THROW_WIN32_ERROR(GetLastError());
        }
        return result;
    }

    int scale_percent = ConvertDPIToScalePercent(dpi);

    auto name_with_scale_percent = AddScalePercentToName(name, scale_percent);
    auto result = FindResource(module, name_with_scale_percent.c_str(), zaf_resource_type_name);
    if (result) {
        return result;
    }

    auto search_table = CreateScalePercentSearchTable(scale_percent);
    for (auto each_percent : search_table) {

        name_with_scale_percent = AddScalePercentToName(name, each_percent);
        auto result = FindResource(module, name_with_scale_percent.c_str(), zaf_resource_type_name);
        if (result) {
            return result;
        }
    }

    ZAF_THROW_WIN32_ERROR(GetLastError());
}


bool ResourceLoader::IsImageResource(const std::wstring& name) {

    auto extension_index = name.find_last_of(L'.');
    if (extension_index == name.npos) {
        return false;
    }

    auto extension = name.substr(extension_index);
    Lowercase(extension);

    constexpr const wchar_t* image_extensions[]{
        L".png",
        L".gif",
        L".jpg",
        L".jpeg",
        L".bmp",
    };

    for (auto each_image_extension : image_extensions) {
        if (extension == each_image_extension) {
            return true;
        }
    }

    return false;
}


int ResourceLoader::ConvertDPIToScalePercent(float dpi) {

    auto percent = static_cast<int>(dpi / 96.f * 100.f);

    for (auto each_percent : SupportedScalePercents) {

        if (percent <= each_percent) {
            return each_percent;
        }
    }

    return SupportedScalePercents[std::size(SupportedScalePercents) - 1];
}


std::wstring ResourceLoader::AddScalePercentToName(const std::wstring& name, int percent) {

    if (percent == 100) {
        return name;
    }

    auto result = name;
    auto extension_index = result.find_last_of(L'.');
    if (extension_index == result.npos) {
        return result;
    }

    result.insert(extension_index, L"_%" + std::to_wstring(percent));
    return result;
}


std::vector<int> ResourceLoader::CreateScalePercentSearchTable(int expected_percent) {

    std::vector<int> result;
    result.reserve(std::size(SupportedScalePercents) - 1); //Not include expected_percent

    std::size_t reverse_index{};
    for (auto each_percent : SupportedScalePercents) {

        if (each_percent == expected_percent) {
            reverse_index = result.size();
        }
        else {
            result.push_back(each_percent);
        }
    }

    std::reverse(result.begin() + reverse_index, result.end());
    std::reverse(result.begin(), result.end());
    return result;
}

}