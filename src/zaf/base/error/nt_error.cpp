#include <zaf/base/error/nt_error.h>

namespace zaf {
namespace {

class NTErrorCategory : public std::error_category {
public:
    const char* name() const noexcept override {
        return "NT";
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

}