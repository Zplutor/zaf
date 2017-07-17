#pragma once

#include <zaf/base/optional.h>
#include <zaf/serialization/deserializer.h>
#include <zaf/serialization/serializer.h>

namespace zaf {

/**
 The check state of a control.
 */
enum class CheckState {

	/**
	 The control is unchecked.
	 */
	Unchecked,

	/**
	 The control is checked.
	 */
	Checked,

	/**
	 The control's check state is indeterminate.
	 */
	Indeterminate,
};


std::wstring ConvertCheckStateToString(CheckState check_state);
optional<CheckState> ConvertCheckStateFromString(const std::wstring& string);


template<>
class Serializer<CheckState> {
public:
    static std::shared_ptr<DataNode> Serialize(const CheckState& check_state) {
        return DataNode::CreateString(ConvertCheckStateToString(check_state));
    }
};


template<>
class Deserializer<CheckState> {
public:
    static bool Deserialize(const DataNode& data_node, CheckState& check_state) {
        auto converted = ConvertCheckStateFromString(data_node.GetString());
        if (converted.has_value()) {
            check_state = converted.value();
        }
        return false;
    }
};

}