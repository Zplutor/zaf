#include <zaf/base/error/com_error.h>

namespace zaf {
namespace {

class ComErrorCategory : public std::error_category {
public:
    const char* name() const noexcept override {
        return "COM";
    }

    std::string message(int condition) const override {
        return std::string{};
    }
};

}

const std::error_category& COMError::Category() {
    static ComErrorCategory category;
    return category;
}


std::error_code COMError::MakeCode(HRESULT hresult) {
    return std::error_code{ static_cast<int>(hresult), Category() };
}

}