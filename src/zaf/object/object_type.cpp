#include <zaf/object/object_type.h>
#include <zaf/object/object_property.h>

namespace zaf {
namespace {

class LinkedObjectParser : public ObjectParser {
public:
    LinkedObjectParser(ObjectParser* self, ObjectParser* base) : self_(self), base_(base) {

    }

    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) override {

        if (base_) {
            base_->ParseFromAttribute(attribute_value, object);
        }

        if (self_) {
            self_->ParseFromAttribute(attribute_value, object);
        }
    }

    void ParseFromNode(const XamlNode& node, Object& object) override {

        if (base_) {
            base_->ParseFromNode(node, object);
        }

        if (self_) {
            self_->ParseFromNode(node, object);
        }
    }

private:
    ObjectParser* self_{};
    ObjectParser* base_{};
};

}


ObjectParser* ObjectType::GetParser() const {

    std::call_once(parser_once_flag_, [this]() {
        InitializeParserLink();
    });
    
    return parser_.get();
}


void ObjectType::InitializeParserLink() const {

    ObjectParser* base_parser{};

    auto base = GetBase();
    if (base) {
        base_parser = base->GetParser();
    }

    parser_ = std::make_unique<LinkedObjectParser>(GetSelfParser(), base_parser);
}


ObjectParser* ObjectType::GetSelfParser() const {
    return nullptr;
}


const std::wstring& ObjectType::GetResourceURI() const {
    static const std::wstring uri{};
    return uri;
}


void ObjectType::RegisterProperty(ObjectProperty* property) {

    auto iterator = std::lower_bound(
        properties_.begin(),
        properties_.end(),
        property,
        [](auto property1, auto property2) {

        return property1->GetName() < property2->GetName();
    });

    properties_.insert(iterator, property);
}


ObjectProperty* ObjectType::FindProperty(std::wstring_view name) const {

    auto iterator = std::lower_bound(
        properties_.begin(),
        properties_.end(), 
        name, 
        [](auto property, std::wstring_view name) {
    
        return property->GetName() < name;
    });

    if (iterator == properties_.end()) {
        return nullptr;
    }

    if ((*iterator)->GetName() != name) {
        return nullptr;
    }

    return *iterator;
}

}