#pragma once

#include <zaf/object/enum_type.h>

namespace zaf::internal {

template<typename T>
class BaseEnumType : public EnumType {
public:
    std::shared_ptr<Object> FindValue(std::wstring_view name) const override {

        static const std::vector<std::pair<std::wstring_view, T>> map = [this]() {
        
            auto map = GenerateMap();
            std::sort(map.begin(), map.end(), [](const auto& item1, const auto& item2) {
                return item1.first < item2.first;
            });
            return map;
        }();

        auto iterator = std::lower_bound(
            map.begin(),
            map.end(),
            name,
            [](const auto& item, std::wstring_view name) {
                return item.first < name;
            }
        );

        if (iterator == map.end()) {
            return nullptr;
        }

        if (iterator->first != name) {
            return nullptr;
        }

        return CreateObject(iterator->second);
    }


    std::wstring GetValueName(const Object& object) override {

        static const std::vector<std::pair<std::wstring_view, T>> map = [this]() {
        
            auto map = GenerateMap();
            std::sort(map.begin(), map.end(), [](const auto& item1, const auto& item2) {
                return item1.second < item2.second;
            });
            return map;
        }();

        auto value = UnboxValue(object);

        auto iterator = std::lower_bound(
            map.begin(),
            map.end(),
            *value,
            [](const auto& item, T value) {
                return item.second < value;
            }
        );

        if (iterator == map.end()) {
            return std::wstring{};
        }

        if (iterator->second != *value) {
            return std::wstring{};
        }

        return std::wstring{ iterator->first.data(), iterator->first.length() };
    }


    virtual void Assign(const Object& source, Object& target) const = 0;

protected:
    virtual std::vector<std::pair<std::wstring_view, T>> GenerateMap() const = 0;
    virtual std::shared_ptr<Object> CreateObject(T value) const = 0;
    virtual const T* UnboxValue(const Object& object) const = 0;
};

}