#include <zaf/base/error/nt_error.h>

namespace zaf {
namespace {

class NTErrorCategory : public std::error_category {
public:
    const char* name() const noexcept override {
        return "zaf.NT";
    }

    std::string message(int condition) const override {
        return std::string{};
    }
};

}

const std::error_category& NTError::Category() {
    static NTErrorCategory category;
    return category;
}


std::error_code NTError::MakeCode(NTSTATUS value) {
    return std::error_code{ static_cast<int>(value), Category() };
}

}