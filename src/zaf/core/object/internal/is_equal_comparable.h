#pragma once

#include <type_traits>

namespace zaf::internal {

template<typename T>
class IsEqualComparable {
private:
    template<typename K>
    static constexpr auto Test(K*) -> decltype(std::declval<K>() == std::declval<K>());

    template<typename K>
    static constexpr int Test(...);

public:
    static constexpr bool Value = std::is_same_v<bool, decltype(Test<T>(nullptr))>;
};


}