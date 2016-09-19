#include "test_check_box_window.h"
#include <zaf/control/check_box.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

using namespace zaf;

void ShowTestCheckBoxWindow() {

	auto window = Create<Window>();
	window->SetTitle(L"text check box");
	window->SetRect(Rect(0, 0, 300, 300));

	auto root_control = window->GetRootControl();

	auto check_box = Create<CheckBox>();
	check_box->SetRect(Rect(0, 0, 100, 60));
	check_box->SetText(L"zaf²âÊÔµÄCheckBox zaf²âÊÔµÄCheckBox");
	root_control->AddChild(check_box);

	auto can_auto_change_check_state = Create<CheckBox>();
	can_auto_change_check_state->SetRect(Rect(0, 100, 300, 30));
	can_auto_change_check_state->SetText(L"Can auto change check state");
	can_auto_change_check_state->SetIsChecked(check_box->CanAutoChangeCheckState());
	can_auto_change_check_state->GetCheckStateChangeEvent().AddListener([check_box](const std::shared_ptr<CheckBox>&) {
		check_box->SetCanAutoChangeCheckState(!check_box->CanAutoChangeCheckState());
	});
	root_control->AddChild(can_auto_change_check_state);

	auto can_be_indeterminate = Create<CheckBox>();
	can_be_indeterminate->SetRect(Rect(0, 130, 300, 30));
	can_be_indeterminate->SetText(L"Can be indeterminate");
	can_be_indeterminate->SetIsChecked(check_box->CanBeIndeterminate());
	can_be_indeterminate->GetCheckStateChangeEvent().AddListener([check_box](const std::shared_ptr<CheckBox>){
		check_box->SetCanBeIndeterminate(!check_box->CanBeIndeterminate());
	});
	root_control->AddChild(can_be_indeterminate);

	window->Show();
}