#pragma once

namespace zaf::internal {

template<typename T>
class BoxedRepresent {
public:
    using ValueType = T;

public:
    BoxedRepresent() = default;
    BoxedRepresent(const T& value) : value_(value) { }

    BoxedRepresent(const BoxedRepresent&) = default;
    BoxedRepresent& operator=(const BoxedRepresent&) = default;

    BoxedRepresent(BoxedRepresent&&) = default;
    BoxedRepresent& operator=(BoxedRepresent&&) = default;

    const T& Value() const {
        return value_;
    }

    T& Value() {
        return value_;
    }

    void SetValue(const T& value) {
        value_ = value;
    }

protected:
    ~BoxedRepresent() = default;

private:
    T value_{};
};

}