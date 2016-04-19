#include "test_label_window.h"
#include <zaf/zaf.h>

using namespace zaf;

void ShowTestLabelWindow() {

	auto window = std::make_shared<Window>();
	window->SetTitle(L"label testing");
	window->SetRect(Rect(0, 0, 300, 300));

	auto label = CreateControl<Label>();
	label->SetRect(Rect(10, 10, 100, 60));
	label->SetIsEnabled(false);
	//label->SetBorderWidth(1);
	//label->SetColor(Label::PaintComponent::Border, Label::PaintState::Normal, Color::Black);
	label->SetText(L"zaf的label控件测试zaf的label控件测试zaf的label控件测试");

	auto root_control = window->GetRootControl();
	root_control->AddChild(label);
	
	window->Show();
}