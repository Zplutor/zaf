#pragma once

#include <optional>

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
std::optional<CheckState> ConvertCheckStateFromString(const std::wstring& string);

}