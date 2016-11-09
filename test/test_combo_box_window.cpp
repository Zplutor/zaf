#include "test_combo_box_window.h"
#include <zaf/control/combo_box.h>
#include <zaf/control/list_box.h>
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

        auto drop_down_list_box = combo_box->GetDropDownListBox();
        for (std::size_t index = 0; index < 10; ++index) {
            drop_down_list_box->AddItemWithText(std::to_wstring(index));
        }
    }
};


void ShowTestComboBoxWindow() {

    auto window = Create<TestComboBoxWindow>();
    window->Show();
}