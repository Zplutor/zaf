#pragma once

#include <zaf/rx/base_subject.h>

namespace zaf::rx {

template<typename T>
class Subject : public BaseSubject<T> {
public:
    Subject() : BaseSubject<T>(internal::CreateSubjectCore()) { }
};

}