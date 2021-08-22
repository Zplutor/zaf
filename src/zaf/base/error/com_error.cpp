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

const std::error_category& COMCategory() {

    static ComErrorCategory category;
    return category;
}

}