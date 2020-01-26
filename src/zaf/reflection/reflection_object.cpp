#include <zaf/reflection/reflection_object.h>
#include <zaf/parsing/parser.h>
#include <zaf/reflection/reflection_type.h>

namespace zaf {
namespace {

class ReflectionObjectParser : public Parser {
public:
    void ParseFromNode(const XamlNode& node, ReflectionObject& reflection_object) override {

    }
};


class ReflectionObjectType : public ReflectionType {
public:
    std::wstring GetName() override {
        return L"ReflectionObject";
    }

    std::shared_ptr<ReflectionObject> CreateInstance() override {
        return std::make_shared<ReflectionObject>();
    }

    std::shared_ptr<Parser> GetParser() override {
        return std::make_shared<ReflectionObjectParser>();
    }

    std::shared_ptr<ReflectionType> GetBase() override {
        return {};
    }
};

}

const std::shared_ptr<ReflectionType> ReflectionObject::Type = 
    std::make_shared<ReflectionObjectType>();

}