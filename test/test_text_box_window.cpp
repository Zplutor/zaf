#include "test_text_box_window.h"
#include <zaf/zaf.h>

#undef max

using namespace zaf;

void ShowTestTextBoxWindow() {

	auto window = std::make_shared<Window>();
	window->SetTitle(L"test text box");
	window->SetRect(Rect(0, 0, 300, 300));

	auto root_control = window->GetRootControl();

	auto text_box = CreateControl<TextBox>();
	text_box->SetRect(Rect(0, 0, 100, 100));
	text_box->SetText(L"TextBoxÎÄ±¾¿ò");
	root_control->AddChild(text_box);

	auto text_label = CreateControl<Label>();
	text_label->SetText(text_box->GetText());
	text_label->SetRect(Rect(100, 0, 100, 100));
	text_box->GetTextChangeEvent().AddListener([text_label](const std::shared_ptr<TextBox>& text_box) {
		text_label->SetText(text_box->GetText());
	});
	root_control->AddChild(text_label);

	auto read_only_check_box = CreateControl<CheckBox>();
	read_only_check_box->SetText(L"Read only");
	read_only_check_box->SetRect(Rect(0, 100, 100, 30));
	read_only_check_box->SetIsChecked(text_box->IsReadOnly());
	read_only_check_box->GetCheckStateChangeEvent().AddListener([text_box](const std::shared_ptr<CheckBox>& check_box) {
		text_box->SetIsReadOnly(check_box->IsChecked());
	});
	root_control->AddChild(read_only_check_box);

	auto max_length_check_box = CreateControl<CheckBox>();
	max_length_check_box->SetText(L"Max length 3");
	max_length_check_box->SetRect(Rect(0, 130, 100, 30));
	max_length_check_box->SetIsChecked(text_box->GetMaximumLength() <= 3);
	max_length_check_box->GetCheckStateChangeEvent().AddListener([text_box](const std::shared_ptr<CheckBox>& check_box) {
		text_box->SetMaximumLength(check_box->IsChecked() ? 3 : std::numeric_limits<std::uint32_t>::max());
	});
	root_control->AddChild(max_length_check_box);

	auto use_password_char_check_box = CreateControl<CheckBox>();
	use_password_char_check_box->SetText(L"Use password character");
	use_password_char_check_box->SetRect(Rect(0, 160, 100, 30));
	use_password_char_check_box->SetIsChecked(text_box->UsePasswordCharacter());
	use_password_char_check_box->GetCheckStateChangeEvent().AddListener([text_box](const std::shared_ptr<CheckBox>& check_box) {
		text_box->SetUsePasswordCharacter(check_box->IsChecked());
	});
	root_control->AddChild(use_password_char_check_box);

	auto password_char_text_box = CreateControl<TextBox>();
	password_char_text_box->SetRect(Rect(100, 160, 100, 30));
	password_char_text_box->SetMaximumLength(1);
	password_char_text_box->GetTextChangeEvent().AddListener([text_box](const std::shared_ptr<TextBox>& password_char_text_box) {
		auto text = password_char_text_box->GetText();
		if (! text.empty()) {
			text_box->SetPasswordCharacter(text[0]);
		}
	});
	root_control->AddChild(password_char_text_box);

	auto multiline_check_box = CreateControl<CheckBox>();
	multiline_check_box->SetText(L"Multiline");
	multiline_check_box->SetRect(Rect(0, 190, 100, 30));
	multiline_check_box->SetIsChecked(text_box->IsMultiline());
	multiline_check_box->GetCheckStateChangeEvent().AddListener([text_box](const std::shared_ptr<CheckBox>& check_box) {
		text_box->SetIsMultiline(check_box->IsChecked());
	});
	root_control->AddChild(multiline_check_box);

	window->Show();
}