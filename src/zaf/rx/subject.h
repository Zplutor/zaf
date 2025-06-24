#pragma once

#include <zaf/rx/internal/subject/base_subject.h>

namespace zaf {

template<typename T>
class Subject : public internal::BaseSubject<T> {
public:
    Subject() : internal::BaseSubject<T>(internal::CreateInnerSubject()) { }
};

}