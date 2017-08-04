#include "property/number_property_item.h"
#include <zaf/base/string/to_numeric.h>
#include <zaf/base/string/to_string.h>
#include "property/text_property_item.h"

namespace {

template<typename ValueType>
std::shared_ptr<PropertyItem> CreateNumberPropertyItem(
    const std::wstring& title,
    const std::function<ValueType()>& get_value,
    const std::function<void(ValueType)>& value_change) {

    auto number_getter = [get_value]() {
        return zaf::ToWideString(get_value(), zaf::ToStringOptions().Precision(2));
    };

    auto number_change = [value_change](const std::wstring& text) {
        ValueType value = 0;
        if (zaf::TryToNumeric<ValueType>(text, value)) {
            value_change(value);
        }
    };

    return CreateTextPropertyItem(title, number_getter, number_change);
}

}

std::shared_ptr<PropertyItem> CreateFloatPropertyItem(
    const std::wstring& title,
    const std::function<float()>& get_value,
    const std::function<void(float)>& value_change) {

    return CreateNumberPropertyItem<float>(title, get_value, value_change);
}


std::shared_ptr<PropertyItem> CreateIntegerPropertyItem(
    const std::wstring& title,
    const std::function<std::int64_t()>& get_value,
    const std::function<void(std::int64_t)>& value_change) {

    return CreateNumberPropertyItem<std::int64_t>(title, get_value, value_change);
}