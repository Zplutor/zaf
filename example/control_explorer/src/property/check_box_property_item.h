#pragma once

#include "property/property_item.h"

std::shared_ptr<PropertyItem> CreateCheckBoxPropertyItem(
    const std::wstring& title,
    const std::function<bool()>& get_value,
    const std::function<void(bool)>& value_change,
    const std::function<void(const std::function<void()>&)>& register_notification);