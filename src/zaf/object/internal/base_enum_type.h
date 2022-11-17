#pragma once

#include <algorithm>
#include <zaf/base/container/utility/sort.h>
#include <zaf/base/container/utility/unique.h>
#include <zaf/object/enum_type.h>

namespace zaf::internal {

template<typename T>
class BaseEnumType : public EnumType {
public:
    std::vector<std::shared_ptr<Object>> GetAllValues() const override {

        std::vector<std::shared_ptr<Object>> result;

        const auto& map = GetMapOrderByValue();
        for (const auto& each_pair : map) {
            result.push_back(CreateObject(each_pair.second));
        }

        return result;
    }


    std::shared_ptr<Object> FindValue(std::wstring_view name) const override {

        const auto& map = GetMapOrderByName();

        auto iterator = std::lower_bound(
            map.begin(),
            map.end(),
            name,
            [](const auto& item, std::wstring_view name) {
                return item.first < name;
            }
        );

        if (iterator == map.end() || iterator->first != name) {
            return nullptr;
        }

        return CreateObject(iterator->second);
    }


    std::wstring GetValueName(const Object& object) const override {

        auto value = UnboxValue(object);

        const auto& map = GetMapOrderByValue();
        auto iterator = std::lower_bound(
            map.begin(),
            map.end(),
            *value,
            [](const auto& item, T value) {
                return item.second < value;
            }
        );

        if (iterator == map.end() || iterator->second != *value) {
            return std::wstring{};
        }

        return std::wstring{ iterator->first.data(), iterator->first.length() };
    }

protected:
    virtual std::vector<std::pair<std::wstring_view, T>> GenerateMap() const = 0;
    virtual std::shared_ptr<Object> CreateObject(T value) const = 0;
    virtual const T* UnboxValue(const Object& object) const = 0;

private:
    const std::vector<std::pair<std::wstring_view, T>>& GetMapOrderByName() const {

        static const std::vector<std::pair<std::wstring_view, T>> map = [this]() {

            auto map = GenerateMap();
            Sort(map, [](const auto& item1, const auto& item2) {
                return item1.first < item2.first;
            });
            return map;
        }();

        return map;
    }

    const std::vector<std::pair<std::wstring_view, T>>& GetMapOrderByValue() const {

        static const std::vector<std::pair<std::wstring_view, T>> map = [this]() {

            auto map = GenerateMap();
            StableSort(map, [](const auto& item1, const auto& item2) {
                return item1.second < item2.second;
            });

            Unique(map, [](const auto& item1, const auto& item2) {
                return item1.second == item2.second;
            });

            return map;
        }();

        return map;
    }
};

}