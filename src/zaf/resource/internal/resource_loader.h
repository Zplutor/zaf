#pragma once

#include <map>
#include <mutex>
#include <zaf/base/stream.h>

namespace zaf::internal {

class ResoureLoader {
public:
    ResoureLoader() = default;
    ~ResoureLoader();

    ResoureLoader(const ResoureLoader&) = delete;
    ResoureLoader& operator=(const ResoureLoader&) = delete;

    Stream Load(const std::wstring& dll, const std::wstring& name);

private:
    HMODULE GetDLLModule(const std::wstring& dll);

private:
    std::mutex lock_;
    std::map<std::wstring, HMODULE> dll_modules_;
};

}