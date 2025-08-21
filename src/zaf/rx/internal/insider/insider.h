#pragma once

namespace zaf::rx::internal {

template<template<typename> typename T>
class Insider;

template<template<typename> typename T>
using InsiderT = typename Insider<T>::Type;

}