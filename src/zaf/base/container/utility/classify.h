#pragma once

namespace zaf {

template<typename C>
struct IsAssociativeContainer {
private:
    template<typename T>
    static constexpr bool Test(typename T::key_type*) {
        return true;
    }

    template<typename T>
    static constexpr bool Test(...) {
        return false;
    }

public:
    static constexpr bool Value = Test<C>(nullptr);
};


template<typename C>
struct IsMapContainer {
private:
    template<typename T>
    static constexpr bool Test(typename T::mapped_type*) {
        return true;
    }

    template<typename T>
    static constexpr bool Test(...) {
        return false;
    }

public:
    static constexpr bool Value = Test<C>(nullptr);
};

}