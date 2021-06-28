#pragma once

namespace zaf::internal {

template<typename T>
class BoxedRepresent {
public:
    BoxedRepresent(const T& value) : value_(value) { }

    const T& GetValue() const {
        return value_;
    }

protected:
    ~BoxedRepresent() = default;

private:
    T value_{};
};

}