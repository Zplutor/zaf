#include "test_radio_button_window.h"
#include <zaf/control/check_box.h>
#include <zaf/control/radio_button.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

using namespace zaf;

void ShowTestRadioButtonWindow() {

	auto window = Create<Window>();
	window->SetTitle(L"test radio button");
	window->SetRect(Rect(0, 0, 300, 300));

	auto root_control = window->GetRootControl();

	auto radio_button1 = Create<RadioButton>();
	radio_button1->SetRect(Rect(0, 0, 100, 60));
	radio_button1->SetText(L"RadioButton1");
	root_control->AddChild(radio_button1);

	auto radio_button2 = Create<RadioButton>();
	radio_button2->SetRect(Rect(100, 0, 100, 60));
	radio_button2->SetText(L"RadioButton2");
	root_control->AddChild(radio_button2);

	auto radio_button3 = Create<RadioButton>();
	radio_button3->SetRect(Rect(200, 0, 100, 60));
	radio_button3->SetText(L"RadioButton3");
	root_control->AddChild(radio_button3);

	auto radio_button_group = std::make_shared<RadioButton::Group>();
	radio_button1->SetGroup(radio_button_group);
	radio_button2->SetGroup(radio_button_group);
	radio_button3->SetGroup(radio_button_group);

	auto auto_select_check_box = Create<CheckBox>();
	auto_select_check_box->SetText(L"Can auto select");
	auto_select_check_box->SetRect(Rect(0, 100, 100, 30));
	auto_select_check_box->SetIsChecked(radio_button1->CanAutoSelect());
	auto_select_check_box->GetCheckStateChangeEvent().AddListener([radio_button1](const std::shared_ptr<CheckBox>& check_box) {
		radio_button1->SetCanAutoSelect(check_box->IsChecked());
	});
	root_control->AddChild(auto_select_check_box);

	window->Show();
}