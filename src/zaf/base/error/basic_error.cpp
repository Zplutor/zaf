#include <zaf/base/error/basic_error.h>

namespace zaf {
namespace {

class BasicErrorCategory : public std::error_category {
public:
    const char* name() const noexcept override {
        return "Basic";
    }

    std::string message(int condition) const override {

        return std::string{};
    }
};

}

const std::error_category& BasicCategory() {
    static BasicErrorCategory category;
    return category;
}

}