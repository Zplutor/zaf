#include <Windows.h>
#include <zaf/application.h>
#include <zaf/base/error.h>
#include <zaf/control/button.h>
#include <zaf/control/layout/array_layouter.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include "test_check_box_window.h"
#include "test_clear_edge_window.h"
#include "test_label_window.h"
#include "test_list_box_window.h"
#include "test_radio_button_window.h"
#include "test_scrollable_control_window.h"
#include "test_text_box_window.h"
#include "test_window_window.h"

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

    std::error_code error_code;
    application.Initialize(error_code);

    if (IsSucceeded(error_code)) {
        application.Run();
    }
}


void BeginRun(Application& application) {

	auto window = Create<Window>();
	window->SetTitle(L"zaf testing");
	window->SetRect(Rect(200, 200, 500, 500));

	static struct {
		const wchar_t* const button_text;
		void(*show_window_function)();
	} show_window_function_map[] = {

        L"Test clear edge", ShowTestClearEdgeWindow,
		L"Test label", ShowTestLabelWindow,
		L"Test check box", ShowTestCheckBoxWindow,
		L"Test radio button", ShowTestRadioButtonWindow,
		L"Test text box", ShowTestTextBoxWindow,
        L"Test scrollable control", ShowTestScrollableControlWindow,
        L"Test list box", ShowTestListBoxWindow,
        L"Test window", ShowTestWindowWindow,
	};

	auto root_control = window->GetRootControl();
	root_control->SetLayouter(GetVerticalArrayLayouter());

	for (const auto& each_item : show_window_function_map) {

		auto button = Create<Button>();
		button->SetText(each_item.button_text);
		button->GetClickEvent().AddListener([each_item](const std::shared_ptr<ClickableControl>&) {
			each_item.show_window_function();
		});
		root_control->AddChild(button);
	}

	window->Show();
    application.SetMainWindow(window);
}