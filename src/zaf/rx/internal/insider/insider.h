#pragma once

namespace zaf::rx::internal {

template<typename T>
class Insider;

template<typename T>
using InsiderT = typename Insider<T>::Type;

}