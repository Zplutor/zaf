#pragma once

#include <zaf/rx/subject/subject_base.h>

namespace zaf::rx {

template<typename T>
class Subject : public SubjectBase<T> {
public:
    Subject() : SubjectBase<T>(internal::CreateSubjectCore()) { }
};

}