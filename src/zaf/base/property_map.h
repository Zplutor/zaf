#pragma once

#include <functional>
#include <map>
#include <zaf/base/any.h>

namespace zaf {

/**
 Reprensents a container that stores name-value properties.

 The types of values stored in PropertyMap must be copyable.
 */
class PropertyMap {
public:
    /**
     Try to get the value of specified property.

     @param PropertyType
         The type of the value expected to get.

     @param property_name
         The name of the property.

     @return 
         A pointer to the value of the property. Return nullptr if the property is not found,
         or the actual type of value doesn't match PropertyType.
     */
	template<typename PropertyType>
	const PropertyType* TryGetProperty(const std::wstring& property_name) const {

		auto iterator = properties_.find(property_name);
		if (iterator == properties_.end()) {
			return nullptr;
		}

        return any_cast<PropertyType>(&iterator->second);;
	}

	template<typename PropertyType>
	PropertyType* TryGetProperty(const std::wstring& property_name) {
		auto property = static_cast<const PropertyMap*>(this)->TryGetProperty<PropertyType>(property_name);
		return const_cast<PropertyType*>(property);
	}

    /**
     Get the value of specified property, or set a new value to the property if the specified value
     cannot be got.

     @param PropertyType
         The type of the value.

     @param property_name
         The name of the property.

     @param property_creator
         A function object used to create a value. The returned value would be set to 
         the property if it is not found or the type of value doesn't match PropertyType.
         This parameter can not be nullptr.

     @return
         A reference to the value of the property.
     */
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

        return *any_cast<PropertyType>(&iterator->second);
    }

    /**
     Set a property with specified name and value.

     @param PropertyType
         The type of value being set.

     @param property_name
         The name of the property.

     @param property_value
         The value of the property.
     */
	template<typename PropertyType>
	void SetProperty(const std::wstring& property_name, const PropertyType& property_value) {
		properties_[property_name] = property_value;
	}

private:
	std::map<std::wstring, any> properties_;
};

}