#pragma once

#include <map>
#include <mutex>
#include <zaf/io/stream/stream.h>

namespace zaf::internal {

class ResourceLoader {
public:
    static bool IsImageResource(const std::wstring& name);
    static int ConvertDPIToScalePercent(float dpi);
    static std::wstring AddScalePercentToName(const std::wstring& name, int percent);
    static std::vector<int> CreateScalePercentSearchTable(int expected_percent);

public:
    ResourceLoader() = default;
    ~ResourceLoader();

    ResourceLoader(const ResourceLoader&) = delete;
    ResourceLoader& operator=(const ResourceLoader&) = delete;

    Stream Load(const std::wstring& dll, const std::wstring& name, float dpi);

private:
    HMODULE GetDLLModule(const std::wstring& dll);
    HRSRC FindResouceMatchesDPI(HMODULE module, const std::wstring& name, float dpi);

private:
    std::mutex lock_;
    std::map<std::wstring, HMODULE> dll_modules_;
};

}