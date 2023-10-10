#pragma once

#include <zaf/rx/internal/subject/base_subject.h>

namespace zaf {

template<typename T>
class Subject : public internal::BaseSubject<T> {
public:
    Subject() : internal::BaseSubject<T>(internal::CreateInnerSubject()) { }
};


template<typename T>
class ReplaySubject : public internal::BaseSubject<T> {
public:
    ReplaySubject() : internal::BaseSubject<T>(internal::CreateInnerSubject()) { }
};

}