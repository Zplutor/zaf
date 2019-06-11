#pragma once

#include <memory>
#include <string>

namespace zaf {

class Parser;
class ReflectionObject;

class ReflectionType {
public:
    virtual ~ReflectionType() = default;
    ReflectionType() = default;
    ReflectionType(const ReflectionType&) = delete;
    ReflectionType& operator=(const ReflectionType&) = delete;

    /**
     Get name of the type.
     */
    virtual std::wstring GetName() = 0;

    /**
     Create an instance of the type.
     */
    virtual std::shared_ptr<ReflectionObject> CreateInstance() = 0;

    /**
     Get the parser for this type.
     */
    virtual std::shared_ptr<Parser> GetParser() = 0;
};

}