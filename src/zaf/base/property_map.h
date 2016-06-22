#pragma once

#include <functional>
#include <map>
#include <zaf/base/variant.h>

namespace zaf {

class PropertyMap {
public:
	template<typename PropertyType>
	const PropertyType* TryGetProperty(const std::wstring& property_name) const {

		auto iterator = properties_.find(property_name);
		if (iterator == properties_.end()) {
			return nullptr;
		}

		return iterator->second.TryCast<PropertyType>();
	}

	template<typename PropertyType>
	PropertyType* TryGetProperty(const std::wstring& property_name) {
		auto property = static_cast<const PropertyMap*>(this)->TryGetProperty<PropertyType>(property_name);
		return const_cast<PropertyType*>(property);
	}

    template<typename PropertyType>
    PropertyType& GetProperty(const std::wstring& property_name, const std::function<PropertyType()>& property_creator) {
    
        auto property = TryGetProperty<PropertyType>(property_name);
        if (property != nullptr) {
            return *property;
        }

        auto iterator = properties_.find(property_name);
        if (iterator == properties_.end()) {
            iterator = properties_.insert(std::make_pair(property_name, property_creator())).first;
        }
        else {
            iterator->second = property_creator();
        }

        return *iterator->second.TryCast<PropertyType>();
    }

	template<typename PropertyType>
	void SetProperty(const std::wstring& property_name, const PropertyType& property_value) {
		properties_[property_name] = Variant(property_value);
	}

private:
	std::map<std::wstring, Variant> properties_;
};

}