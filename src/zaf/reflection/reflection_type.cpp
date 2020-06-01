#include <zaf/reflection/reflection_type.h>
#include <zaf/parsing/parser.h>

namespace zaf {
namespace {

class DumbParser : public Parser {
public:
    void ParseFromNode(const XamlNode& node, ReflectionObject& reflection_object) override { }
};

}

std::shared_ptr<Parser> ReflectionType::GetParser() {

    auto base_type = GetBase();
    if (base_type) {
        return base_type->GetParser();
    }

    static auto dump_parser = std::make_shared<DumbParser>();
    return dump_parser;
}

}