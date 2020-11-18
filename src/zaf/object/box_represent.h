#pragma once

namespace zaf {

template<typename T>
class BoxRepresent {
public:
    BoxRepresent(const T& value) : value_(value) { }

    const T& GetValue() const {
        return value_;
    }

protected:
    ~BoxRepresent() = default;

private:
    T value_{};
};

}