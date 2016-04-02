#include "test_text_box_window.h"
#include <zaf/zaf.h>

#undef max

using namespace zaf;

static void InitializeTextAlignmentControls(const std::shared_ptr<TextBox>& text_box, const std::shared_ptr<Control>& root_control);

void ShowTestTextBoxWindow() {

	auto window = std::make_shared<Window>();
	window->SetTitle(L"test text box");
	window->SetRect(Rect(0, 0, 500, 500));

	auto root_control = window->GetRootControl();

	auto text_box = CreateControl<TextBox>();
	text_box->SetRect(Rect(0, 0, 200, 100));
	text_box->SetText(L"TextBoxÎÄ±¾¿ò");
	root_control->AddChild(text_box);

	auto text_label = CreateControl<Label>();
	text_label->SetText(text_box->GetText());
	text_label->SetRect(Rect(200, 0, 100, 100));
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

	InitializeTextAlignmentControls(text_box, root_control);

	auto word_wrap_check_box = CreateControl<CheckBox>();
	word_wrap_check_box->SetText(L"Word wrap");
	word_wrap_check_box->SetRect(Rect(0, 250, 100, 30));
	word_wrap_check_box->SetIsChecked(text_box->GetWordWrapping() != WordWrapping::NoWrap);
	word_wrap_check_box->GetCheckStateChangeEvent().AddListener([text_box](const std::shared_ptr<CheckBox>& check_box) {
		text_box->SetWordWrapping(check_box->IsChecked() ? WordWrapping::Wrap : WordWrapping::NoWrap);
	});
	root_control->AddChild(word_wrap_check_box);

	window->Show();
}


static void InitializeTextAlignmentControls(const std::shared_ptr<TextBox>& text_box, const std::shared_ptr<Control>& root_control) {

	auto label = CreateControl<Label>();
	label->SetText(L"TextAlignemnt: ");
	label->SetRect(Rect(0, 220, 100, 30));
	root_control->AddChild(label);

	auto select_change_event_callback = [text_box](const std::shared_ptr<RadioButton>& radio_button, TextAlignment text_alignment) {
		if (radio_button->IsSelected()) {
			text_box->SetTextAlignment(text_alignment);
		}
	};

	auto group = std::make_shared<RadioButton::Group>();

	auto left_radio_button = CreateControl<RadioButton>();
	left_radio_button->SetText(L"Left");
	left_radio_button->SetGroup(group);
	left_radio_button->SetRect(Rect(100, 220, 100, 30));
	left_radio_button->GetSelectStateChangeEvent().AddListener(
		std::bind(select_change_event_callback, std::placeholders::_1, TextAlignment::Leading)
	);
	root_control->AddChild(left_radio_button);

	auto center_radio_button = CreateControl<RadioButton>();
	center_radio_button->SetText(L"Center");
	center_radio_button->SetGroup(group);
	center_radio_button->SetRect(Rect(200, 220, 100, 30));
	center_radio_button->GetSelectStateChangeEvent().AddListener(
		std::bind(select_change_event_callback, std::placeholders::_1, TextAlignment::Center)
	);
	root_control->AddChild(center_radio_button);

	auto right_radio_button = CreateControl<RadioButton>();
	right_radio_button->SetText(L"Right");
	right_radio_button->SetGroup(group);
	right_radio_button->SetRect(Rect(300, 220, 100, 30));
	right_radio_button->GetSelectStateChangeEvent().AddListener(
		std::bind(select_change_event_callback, std::placeholders::_1, TextAlignment::Tailing)
	);
	root_control->AddChild(right_radio_button);
}