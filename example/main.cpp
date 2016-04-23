#include <Windows.h>
#include <zaf/zaf.h>

using namespace zaf;

static int g_position_y = 100;

void OnBeginRun(Application&);

int WINAPI WinMain(
	HINSTANCE /* hInstance */,
	HINSTANCE /* hPrevInstance */,
	LPSTR /* lpCmdLine */,
	int /* nCmdShow */
	) {
	
	auto& application = Application::GetInstance();
	application.GetBeginRunEvent().AddListener(OnBeginRun);

	if (application.Initialize()) {
		application.Run();
	}

	return 0;
}


void OnBeginRun(Application&) {

	auto window = std::make_shared<Window>();
	window->SetTitle(L"Zaf");
	window->SetRect(Rect(200, 200, 500, 500));
	window->Show();

	auto root_control = window->GetRootControl();

	auto scrollable_control = CreateControl<ScrollableControl>();
	scrollable_control->SetRect(Rect(Point(), root_control->GetSize()));
	scrollable_control->SetAnchor(Anchor::Left | Anchor::Top | Anchor::Right | Anchor::Bottom);
	scrollable_control->SetScrollAreaSize(Size(500, 500));
	window->GetRootControl()->AddChild(scrollable_control);

	auto content_control = scrollable_control->GetScrolledControl();

	auto control1 = CreateControl<Button>();
	control1->SetRect(Rect(-50, 10, 100, 50));
	control1->SetText(L"Blue");
	content_control->AddChild(control1);

	auto control2 = CreateControl<Button>();
	control2->SetRect(Rect(20, 20, 20, 20));
	control2->SetText(L"Red");
	content_control->AddChild(control2);

	auto control3 = CreateControl<Button>();
	control3->SetRect(Rect(50, 20, 20, 80));
	control3->SetText(L"Lime");
	content_control->AddChild(control3);

	auto control4 = CreateControl<Button>();
	control4->SetRect(Rect(5, 5, 70, 10));
	control4->SetText(L"Black");
	control3->AddChild(control4);

	auto check_box = CreateControl<CheckBox>();
	check_box->SetRect(Rect(100, 80, 100, 30));
	check_box->SetText(L"CheckBox");
	content_control->AddChild(check_box);

	auto disabled_button = CreateControl<Button>();
	disabled_button->SetText(L"禁用的按钮");
	disabled_button->SetIsEnabled(false);
	disabled_button->SetRect(Rect(100, 30, 100, 30));
	content_control->AddChild(disabled_button);

	auto invisible_button = CreateControl<Button>();
	invisible_button->SetText(L"不可见的按钮");
	invisible_button->SetIsVisible(true);
	invisible_button->SetRect(Rect(200, 30, 100, 30));
	content_control->AddChild(invisible_button);

	auto button = CreateControl<Button>();
	button->SetRect(Rect(200, 200, 100, 30));
	button->SetText(L"Button");
	button->SetAnchor(Anchor::Bottom);
	button->GetClickEvent().AddListener([window, disabled_button, invisible_button](const std::shared_ptr<ClickableControl>&) {

		auto new_control = CreateControl<Label>();
		new_control->SetRect(Rect(0, g_position_y, 40, 10));
		new_control->SetText(std::to_wstring(g_position_y));
		new_control->SetIsEnabled(g_position_y % 2 == 0);
		window->GetRootControl()->AddChild(new_control);

		g_position_y += 11;

		disabled_button->SetIsEnabled(! disabled_button->IsEnabled());
		invisible_button->SetIsVisible(! invisible_button->IsVisible());
	});
	content_control->AddChild(button);

	auto text_box = CreateControl<TextBox>();
	text_box->SetRect(Rect(150, 250, 100, 40));
	text_box->SetText(L"TextBox 文本框");
	content_control->AddChild(text_box);
}