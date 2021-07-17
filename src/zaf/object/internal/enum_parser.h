#pragma once

#include <zaf/object/internal/base_enum_type.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_utility.h>

namespace zaf::internal {

template<typename T>
class EnumParser : public ObjectParser {
public:
    explicit EnumParser(const BaseEnumType<T>* type) : enum_type_(type) {

    }


    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) {

        Parse(attribute_value, object);
    }


    void ParseFromNode(const XamlNode& node, Object& object) {

        auto content_string = GetContentStringFromXamlNode(node);
        if (!content_string) {
            return;
        }

        Parse(*content_string, object);
    }

private:
    void Parse(const std::wstring& text, Object& object) {

        auto value_object = enum_type_->FindValue(text);
        if (!value_object) {
            //TODO: throw error
        }

        enum_type_->Assign(*value_object, object);
    }

private:
    const BaseEnumType<T>* enum_type_{};
};

}