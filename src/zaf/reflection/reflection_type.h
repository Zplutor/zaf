#pragma once

#include <memory>
#include <string>

namespace zaf {

class Parser;
class ReflectionObject;

class ReflectionType {
public:
    ReflectionType() = default;
    virtual ~ReflectionType() = default;

    ReflectionType(const ReflectionType&) = delete;
    ReflectionType& operator=(const ReflectionType&) = delete;

    /**
     Get base type of the type.

     Return nullptr if there is no base type.
     */
    virtual std::shared_ptr<ReflectionType> GetBase() = 0;

    /**
     Get name of the type.
     */
    virtual std::wstring GetName() = 0;

    /**
     Create an instance of the type.
     */
    virtual std::shared_ptr<ReflectionObject> CreateInstance() = 0;

    /**
     Get the parser for the type.
     */
    virtual std::shared_ptr<Parser> GetParser();

    virtual std::wstring GetResourceUri() {
        return {};
    }
};

}