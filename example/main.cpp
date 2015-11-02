#include <Windows.h>
#include <zaf/zaf.h>
#include <zaf/internal/log.h>

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
	application.Initialize();
	application.OnBeginRun.AddListener(OnBeginRun);

	application.Run();

	return 0;
}


void OnBeginRun(Application&) {

	auto window = std::make_shared<Window>();
	window->SetTitle(L"Zaf");
	window->SetRect(Rect(200, 200, 500, 500));
	window->Show();

	auto control1 = std::make_shared<Button>();
	control1->SetRect(Rect(-50, 10, 100, 50));
	control1->SetText(L"Blue");
	window->GetRootControl()->AddChild(control1);

	auto control2 = std::make_shared<Button>();
	control2->SetRect(Rect(20, 20, 20, 20));
	control2->SetText(L"Red");
	window->GetRootControl()->AddChild(control2);

	auto control3 = std::make_shared<Button>();
	control3->SetRect(Rect(50, 20, 20, 80));
	control3->SetText(L"Lime");
	window->GetRootControl()->AddChild(control3);

	auto control4 = std::make_shared<Button>();
	control4->SetRect(Rect(5, 5, 70, 10));
	control4->SetText(L"Black");
	control3->AddChild(control4);

	auto disabled_button = std::make_shared<Button>();
	disabled_button->SetText(L"½ûÓÃµÄ°´Å¥");
	disabled_button->SetIsEnabled(false);
	disabled_button->SetRect(Rect(100, 30, 100, 30));
	window->GetRootControl()->AddChild(disabled_button);

	auto button = std::make_shared<Button>();
	button->SetRect(Rect(200, 200, 100, 30));
	button->SetText(L"Button");
	button->AddAnchor(Control::Anchor::Bottom);
	button->OnClick.AddListener([window, disabled_button](const std::shared_ptr<Button>&) {

		auto new_control = std::make_shared<Label>();
		new_control->SetRect(Rect(0, g_position_y, 40, 10));
		new_control->SetText(std::to_wstring(g_position_y));
		window->GetRootControl()->AddChild(new_control);

		g_position_y += 11;

		disabled_button->SetIsEnabled(! disabled_button->IsEnabled());
	});
	window->GetRootControl()->AddChild(button);

	auto scrollable_control = std::make_shared<ScrollableControl>();
	scrollable_control->SetRect(Rect(100, 100, 300, 300));
	scrollable_control->SetContentSize(Size(500, 500));
	window->GetRootControl()->AddChild(scrollable_control);

	auto content_control = scrollable_control->GetContentControl();
	auto b1 = std::make_shared<Button>();
	b1->SetRect(Rect(0, 0, 50, 50));
	b1->SetText(L"b1");
	auto b2 = std::make_shared<Button>();
	b2->SetRect(Rect(50, 0, 50, 50));
	b2->SetText(L"b2");
	auto b3 = std::make_shared<Button>();
	b3->SetRect(Rect(450, 0, 50, 50));
	b3->SetText(L"b3");
	auto b4 = std::make_shared<Button>();
	b4->SetRect(Rect(450, 450, 50, 50));
	b4->SetText(L"b4");
	content_control->AddChild(b1);
	content_control->AddChild(b2);
	content_control->AddChild(b3);
	content_control->AddChild(b4);
}