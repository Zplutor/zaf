#pragma once

#include <string>

namespace zaf {

class Object {
public:
    static const std::shared_ptr<Object>& Empty();

public:
    Object() = default;
    virtual ~Object() = default;

    virtual std::wstring ToString() const;
};

}