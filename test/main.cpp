#include <Windows.h>
#include <zaf/zaf.h>
#include "test_check_box_window.h"
#include "test_label_window.h"
#include "test_radio_button_window.h"
#include "test_text_box_window.h"

using namespace zaf;

void BeginRun(Application&);

int WINAPI WinMain(
	HINSTANCE /* hInstance */,
	HINSTANCE /* hPrevInstance */,
	LPSTR /* lpCmdLine */,
	int /* nCmdShow */
) {

	auto& application = Application::GetInstance();
	application.GetBeginRunEvent().AddListener(BeginRun);
	if (application.Initialize()) {
		application.Run();
	}
}


void BeginRun(Application&) {

	auto window = std::make_shared<Window>();
	window->SetTitle(L"zaf testing");
	window->SetRect(Rect(200, 200, 500, 500));

	static struct {
		const wchar_t* const button_text;
		void(*show_window_function)();
	} show_window_function_map[] = {

		L"Test label", ShowTestLabelWindow,
		L"Test check box", ShowTestCheckBoxWindow,
		L"Test radio button", ShowTestRadioButtonWindow,
		L"Test text box", ShowTestTextBoxWindow,
	};

	auto root_control = window->GetRootControl();

	float y = 0;
	for (const auto& each_item : show_window_function_map) {

		auto button = std::make_shared<Button>();
		button->SetText(each_item.button_text);
		button->SetRect(Rect(0, y, 100, 30));
		button->GetClickEvent().AddListener([each_item](const std::shared_ptr<ClickableControl>&) {
			each_item.show_window_function();
		});
		root_control->AddChild(button);
		y += 30;
	}

	window->Show();
}