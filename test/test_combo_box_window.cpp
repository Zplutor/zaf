#include "test_combo_box_window.h"
#include <zaf/control/combo_box.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

using namespace zaf;

class TestComboBoxWindow : public Window {
public:
    void Initialize() override {

        __super::Initialize();

        SetTitle(L"test combo box");

        auto combo_box = Create<ComboBox>();
        combo_box->SetRect(Rect(0, 0, 100, 30));
        GetRootControl()->AddChild(combo_box);
    }
};


void ShowTestComboBoxWindow() {

    auto window = Create<TestComboBoxWindow>();
    window->Show();
}