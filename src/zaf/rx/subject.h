#pragma once

#include <zaf/rx/internal/subject/base_subject.h>

namespace zaf {

template<typename T>
class Subject : public rx::internal::BaseSubject<T> {
public:
    Subject() : rx::internal::BaseSubject<T>(rx::internal::CreateSubjectCore()) { }
};

}