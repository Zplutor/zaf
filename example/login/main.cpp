#include <zaf/application.h>
#include <zaf/control/button.h>
#include <zaf/control/label.h>
#include <zaf/control/text_box.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

static void OnBeginRun(const zaf::ApplicationBeginRunInfo&);
static std::shared_ptr<zaf::Window> CreateMainWindow();
static std::vector<std::shared_ptr<zaf::Control>> CreateControls();
static void OnSignInClick(
    const std::shared_ptr<zaf::TextBox>& account_text_box, 
    const std::shared_ptr<zaf::TextBox>& password_text_box,
    const std::shared_ptr<zaf::Label>& sign_in_result_label);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    zaf::Application& application = zaf::Application::Instance();

    //Initialize the application.
    zaf::InitializeParameters initialize_parameters;
    application.Initialize(initialize_parameters);

    //Register the begin run event, do something when the event triggers.
    application.Subscriptions() += application.BeginRunEvent().Subscribe(OnBeginRun);

    //Begin to run.
    application.Run();
    return 0;
}


static void OnBeginRun(const zaf::ApplicationBeginRunInfo&) {

    //Create a main window.
    std::shared_ptr<zaf::Window> main_window = CreateMainWindow();

    //Set this window as the main window of the application.
    zaf::Application::Instance().SetMainWindow(main_window);

    //Show the window.
    main_window->Show();
}


static std::shared_ptr<zaf::Window> CreateMainWindow() {

    //Create a window and set its properties.
    std::shared_ptr<zaf::Window> main_window = zaf::Create<zaf::Window>();
    main_window->SetTitle(L"Login");
    main_window->SetSize(zaf::Size(300, 200));

    //Create controls and add them to the root control of window.
    std::vector<std::shared_ptr<zaf::Control>> controls = CreateControls();

    for (const auto& each_control : controls) {
        main_window->RootControl()->AddChild(each_control);
    }

    return main_window;
}


static std::vector<std::shared_ptr<zaf::Control>> CreateControls() {

    //Account label.
    auto account_label = zaf::Create<zaf::Label>();
    account_label->SetText(L"Account: ");
    account_label->SetRect(zaf::Rect(30, 30, 80, 20));

    //Account text box.
    auto account_text_box = zaf::Create<zaf::TextBox>();
    account_text_box->SetRect(zaf::Rect(110, 30, 140, 20));

    //Password label.
    auto password_label = zaf::Create<zaf::Label>();
    password_label->SetText(L"Password: ");
    password_label->SetRect(zaf::Rect(30, 60, 80, 20));

    //Password text box.
    auto password_text_box = zaf::Create<zaf::TextBox>();
    password_text_box->SetUsePasswordCharacter(true);
    password_text_box->SetPasswordCharacter(L'*');
    password_text_box->SetRect(zaf::Rect(110, 60, 140, 20));

    //Sign in result label.
    auto sign_in_result_label = zaf::Create<zaf::Label>();
    sign_in_result_label->SetRect(zaf::Rect(40, 115, 80, 30));

    //Sign in button.
    auto sign_in_button = zaf::Create<zaf::Button>();
    sign_in_button->SetText(L"Sign in");
    sign_in_button->SetRect(zaf::Rect(110, 110, 80, 30));
    sign_in_button->Subscriptions() += sign_in_button->ClickEvent().Subscribe(std::bind(
        OnSignInClick, 
        account_text_box, 
        password_text_box, 
        sign_in_result_label));

    //Return controls.
    return {
        account_label,
        account_text_box,
        password_label,
        password_text_box,
        sign_in_result_label,
        sign_in_button,
    };
}


static void OnSignInClick(
    const std::shared_ptr<zaf::TextBox>& account_text_box,
    const std::shared_ptr<zaf::TextBox>& password_text_box,
    const std::shared_ptr<zaf::Label>& sign_in_result_label) {

    if (account_text_box->Text() == L"zplutor" &&
        password_text_box->Text() == L"123456") {

        sign_in_result_label->SetTextColor(zaf::Color::Green());
        sign_in_result_label->SetText(L"OK");
    }
    else {

        sign_in_result_label->SetTextColor(zaf::Color::Red());
        sign_in_result_label->SetText(L"Wrong");
    }
}