#pragma once

#include <typeinfo>
#include <zaf/base/error/error.h>

namespace zaf {

class InvalidCastError : public std::bad_cast, public Error {
public:

};

}