#pragma once

#include <type_traits>

namespace zaf {

template<typename T>
struct GetCustomBoxedType {
    
};

template<typename T>
using GetCustomBoxedTypeT = typename GetCustomBoxedType<T>::type;

}