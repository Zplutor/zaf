#pragma once

#include <optional>
#include <unordered_set>
#include <variant>
#include <zaf/dynamic/object_property.h>

namespace zaf {

class PropertyValuePair {
public:
    struct Hash {
        std::size_t operator()(const PropertyValuePair& pair) const {
            return std::hash<std::wstring_view>()(pair.PropertyName());
        }
    };

    struct Equal {
        bool operator()(const PropertyValuePair& pair1, const PropertyValuePair& pair2) const {
            return pair1.PropertyName() == pair2.PropertyName();
        }
    };

public:
    PropertyValuePair(ObjectProperty* property, std::shared_ptr<Object> value);
    PropertyValuePair(std::wstring property_name, std::wstring value);

    std::wstring_view PropertyName() const noexcept;

    void SetTo(Object& object) const;
    bool IsSetIn(const Object& object) const;

private:
    struct ParsedData {
        ObjectProperty* property{};
        std::shared_ptr<Object> value;
    };

    struct NotParsedData {
        std::wstring property_name;
        std::wstring value;
    };

private:
    static void SetWithParsedData(const ParsedData& data, Object& object);
    static void SetWithNotParsedData(const NotParsedData& data, Object& object);

    static bool IsSetWithParsedData(const ParsedData& data, const Object& object);
    static bool IsSetWithNotParsedData(const NotParsedData& data, const Object& object);

    static std::optional<ParsedData> ConvertToParsedData(
        const NotParsedData& data,
        const Object& object);

private:
    std::variant<ParsedData, NotParsedData> data_;
};

using PropertyValuePairSet = std::unordered_set<
    PropertyValuePair, 
    PropertyValuePair::Hash, 
    PropertyValuePair::Equal
>;

}