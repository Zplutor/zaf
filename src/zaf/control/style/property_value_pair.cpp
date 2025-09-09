#include <zaf/control/style/property_value_pair.h>
#include <zaf/dynamic/object.h>

namespace zaf {

PropertyValuePair::PropertyValuePair(ObjectProperty* property, std::shared_ptr<dynamic::Object> value) :
    data_(ParsedData{ property, std::move(value) }) {

}


PropertyValuePair::PropertyValuePair(std::wstring property_name, std::wstring value) :
    data_(NotParsedData{ std::move(property_name), std::move(value) }) {

}


std::wstring_view PropertyValuePair::PropertyName() const noexcept {

    auto not_parsed_data = std::get_if<NotParsedData>(&data_);
    if (not_parsed_data) {
        return not_parsed_data->property_name;
    }

    auto& parsed_data = std::get<ParsedData>(data_);
    return parsed_data.property->Name();
}


void PropertyValuePair::SetTo(dynamic::Object& object) const {

    std::visit([&object](const auto& data) {

        using DataType = std::decay_t<decltype(data)>;

        if constexpr (std::is_same_v<DataType, ParsedData>) {
            SetWithParsedData(data, object);
        }
        else if constexpr (std::is_same_v<DataType, NotParsedData>) {
            SetWithNotParsedData(data, object);
        }
    },
    data_);
}


void PropertyValuePair::SetWithParsedData(const ParsedData& data, dynamic::Object& object) {
    data.property->SetValue(object, data.value);
}


void PropertyValuePair::SetWithNotParsedData(const NotParsedData& data, dynamic::Object& object) {
    auto parsed_data = ConvertToParsedData(data, object);
    if (parsed_data) {
        SetWithParsedData(*parsed_data, object);
    }
}


bool PropertyValuePair::IsSetIn(const dynamic::Object& object) const {

    bool result{};
    std::visit([&object, &result](const auto& data) {

        using DataType = std::decay_t<decltype(data)>;

        if constexpr (std::is_same_v<DataType, ParsedData>) {
            result = IsSetWithParsedData(data, object);
        }
        else if constexpr (std::is_same_v<DataType, NotParsedData>) {
            result = IsSetWithNotParsedData(data, object);
        }
    },
    data_);

    return result;
}


bool PropertyValuePair::IsSetWithParsedData(
    const ParsedData& data,
    const dynamic::Object& object) {

    auto value = data.property->GetValue(object);
    if (!value) {
        return false;
    }

    return value->IsEqual(*data.value);
}


bool PropertyValuePair::IsSetWithNotParsedData(
    const NotParsedData& data, 
    const dynamic::Object& object) {

    auto parsed_data = ConvertToParsedData(data, object);
    if (parsed_data) {
        return IsSetWithParsedData(*parsed_data, object);
    }
    return false;
}


std::optional<PropertyValuePair::ParsedData> PropertyValuePair::ConvertToParsedData(
    const NotParsedData& data,
    const dynamic::Object& object) {

    auto type = object.DynamicType();

    auto property = type->GetProperty(data.property_name);
    if (!property) {
        return std::nullopt;
    }

    auto value_type = property->ValueType();
    auto value = value_type->CreateInstance();
    value_type->Parser()->ParseFromAttribute(data.value, *value);
    return ParsedData{ property, std::move(value) };
}

}