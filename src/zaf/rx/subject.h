#pragma once

#include <zaf/rx/base_subject.h>

namespace zaf {

template<typename T>
class Subject : public rx::BaseSubject<T> {
public:
    Subject() : rx::BaseSubject<T>(rx::internal::CreateSubjectCore()) { }
};

}