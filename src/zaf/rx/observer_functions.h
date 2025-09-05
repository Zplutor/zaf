#pragma once

/**
@file
    Defines the emission handler types.
*/

#include <exception>
#include <functional>

namespace zaf::rx {

/**
The type of handlers for item emission.
*/
template<typename T>
using OnNext = std::function<void(const T&)>;

/**
The type of handlers for error emission.
*/
using OnError = std::function<void(const std::exception_ptr&)>;

/**
The type of handlers for completion emission.
*/
using OnCompleted = std::function<void()>;

}