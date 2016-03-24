#include "test_radio_button_window.h"
#include <zaf/zaf.h>

using namespace zaf;

void ShowTestRadioButtonWindow() {

	auto window = std::make_shared<Window>();
	window->SetTitle(L"test radio button");
	window->SetRect(Rect(0, 0, 300, 300));

	auto root_control = window->GetRootControl();

	auto radio_button1 = std::make_shared<RadioButton>();
	radio_button1->SetRect(Rect(0, 0, 100, 60));
	radio_button1->SetText(L"RadioButton1");
	root_control->AddChild(radio_button1);

	auto radio_button2 = std::make_shared<RadioButton>();
	radio_button2->SetRect(Rect(100, 0, 100, 60));
	radio_button2->SetText(L"RadioButton2");
	root_control->AddChild(radio_button2);

	auto radio_button3 = std::make_shared<RadioButton>();
	radio_button3->SetRect(Rect(200, 0, 100, 60));
	radio_button3->SetText(L"RadioButton3");
	root_control->AddChild(radio_button3);

	auto radio_button_group = std::make_shared<RadioButton::Group>();
	radio_button1->SetGroup(radio_button_group);
	radio_button2->SetGroup(radio_button_group);
	radio_button3->SetGroup(radio_button_group);

	window->Show();
}