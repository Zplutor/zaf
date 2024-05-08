#include <zaf/base/error/win32_error.h>

namespace zaf {
namespace {

class Win32ErrorCategory: public std::error_category {
public:
    const char* name() const noexcept override {
        return "zaf.Win32";
    }

    std::string message(int condition) const override {
        return std::string{};
    }
};

}

const std::error_category& Win32Error::Category() {
    static Win32ErrorCategory category;
    return category;
}


std::error_code Win32Error::MakeCode(DWORD code) {
    return std::error_code{ static_cast<int>(code), Category() };
}

}