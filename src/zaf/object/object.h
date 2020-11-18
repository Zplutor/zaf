#pragma once

#include <string>

namespace zaf {

class Object {
public:
    static const std::shared_ptr<Object>& Empty();

public:
    Object() = default;
    virtual ~Object() = default;

    virtual bool IsEqual(const Object& other) const;
    virtual std::size_t Hash() const;

    virtual std::wstring ToString() const;
};

}