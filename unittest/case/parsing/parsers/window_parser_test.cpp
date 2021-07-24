#include <gtest/gtest.h>
#include <zaf/control/button.h>
#include <zaf/control/label.h>
#include <zaf/window/window.h>
#include "utility.h"

TEST(WindowParser, ParseIsPopup) {

    ASSERT_TRUE(TestBooleanProperty<zaf::Window>("IsPopup", [](zaf::Window& window) {
        return window.IsPopup();
    }));
}


TEST(WindowParser, ParseInitialRectStyle) {

    bool result = TestEnumProperty<zaf::Window, zaf::Window::InitialRectStyle>(
        "InitialRectStyle",
        {
            { zaf::Window::InitialRectStyle::CenterInScreen, "CenterInScreen" },
            { zaf::Window::InitialRectStyle::CenterInOwner, "CenterInOwner" },
            { zaf::Window::InitialRectStyle::Custom, "Custom" },
        },
        [](zaf::Window& window) {
            return window.GetInitialRectStyle();
        }
    );
    ASSERT_TRUE(result);
}


TEST(WindowParser, ParseHasBorder) {

    ASSERT_TRUE(TestBooleanProperty<zaf::Window>("HasBorder", [](zaf::Window& window) {
        return window.HasBorder();
    }));
};


TEST(WindowParser, ParseHasTitleBar) {
    
    //HasTitleBar depends on IsPopup and HasBorder.

    auto window = zaf::CreateObjectFromXaml<zaf::Window>(R"(
        <Window HasBorder="false" HasTitleBar="true" />
    )");
    ASSERT_TRUE(window->HasTitleBar());

    window = zaf::CreateObjectFromXaml<zaf::Window>(R"(
        <Window HasBorder="false" HasTitleBar="false" />
    )");
    ASSERT_FALSE(window->HasTitleBar());
};


TEST(WindowParser, ParseActivateOption) {

    auto window = zaf::CreateObjectFromXaml<zaf::Window>(R"(
        <Window ActivateOption="NoActivate DiscardMouseMessage" />
    )");
    ASSERT_EQ(window->GetActivateOption(), 
        zaf::Window::ActivateOption::NoActivate | 
        zaf::Window::ActivateOption::DiscardMouseMessage);

    window = zaf::CreateObjectFromXaml<zaf::Window>(R"(
        <Window>
            <Window.ActivateOption>
                NoActivate 
                DiscardMouseMessage
            </Window.ActivateOption>
        </Window>
    )");
    ASSERT_EQ(window->GetActivateOption(),
        zaf::Window::ActivateOption::NoActivate |
        zaf::Window::ActivateOption::DiscardMouseMessage);
}


TEST(WindowParser, ParseTitle) {

    auto xaml = R"(<Window Title="xxx"></Window>)";
    auto window = zaf::CreateObjectFromXaml<zaf::Window>(xaml); 
    ASSERT_EQ(window->GetTitle(), L"xxx");

    xaml = R"(<Window><Window.Title>yyy</Window.Title></Window>)";
    window = zaf::CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetTitle(), L"yyy");
}


TEST(WindowParser, ParseWidthAndHeight) {

    auto xaml = R"(<Window Width="1" Height="2"></Window>)";
    auto window = zaf::CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetWidth(), 1);
    ASSERT_EQ(window->GetHeight(), 2);

    xaml = R"(
        <Window>
            <Window.Width>3</Window.Width>
            <Window.Height>4</Window.Height>
        </Window>
    )";
    window = zaf::CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetWidth(), 3);
    ASSERT_EQ(window->GetHeight(), 4);
}


TEST(WindowParser, ParseMinWidthAndHeight) {

    auto xaml = R"(<Window MinWidth="1" MinHeight="2"></Window>)";
    auto window = zaf::CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetMinWidth(), 1);
    ASSERT_EQ(window->GetMinHeight(), 2);

    xaml = R"(
        <Window>
            <Window.MinWidth>3</Window.MinWidth>
            <Window.MinHeight>4</Window.MinHeight>
        </Window>
    )";
    window = zaf::CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetMinWidth(), 3);
    ASSERT_EQ(window->GetMinHeight(), 4);
}


TEST(WindowParser, ParsePosition) {

    auto xaml = R"(<Window Position="5,7"></Window>)";
    auto window = zaf::CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetPosition(), zaf::Point(5, 7));

    xaml = R"(
        <Window>
            <Window.Position X="8" Y="9"></Window.Position>
        </Window>
    )";
    window = zaf::CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetPosition(), zaf::Point(8, 9));
}


TEST(WindowParser, ParseSize) {

    auto xaml = R"(<Window Size="10,20"></Window>)";
    auto window = zaf::CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetSize(), zaf::Size(10, 20));

    xaml = R"(
        <Window>
            <Window.Size Width="30" Height="40"></Window.Size>
        </Window>
    )";
    window = zaf::CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetSize(), zaf::Size(30, 40));
}


TEST(WindowParser, ParseClientSize) {

    auto xaml = R"(<Window ClientSize="10,20"></Window>)";
    auto window = zaf::CreateObjectFromXaml<zaf::Window>(xaml);

    auto window_size = window->GetSize();
    ASSERT_GT(window_size.width, 10);
    ASSERT_GT(window_size.height, 20);

    xaml = R"(
        <Window>
            <Window.ClientSize Width="30" Height="40"></Window.ClientSize>
        </Window>
    )";
    window = zaf::CreateObjectFromXaml<zaf::Window>(xaml);
    window_size = window->GetSize();
    ASSERT_GT(window_size.width, 30);
    ASSERT_GT(window_size.height, 40);
}


TEST(WindowParser, ParseRect) {

    auto window = zaf::CreateObjectFromXaml<zaf::Window>(R"(
        <Window>
            <Window.Rect Position="1,2" Size="3,4"></Window.Rect>
        </Window>
    )");
    ASSERT_EQ(window->GetRect(), zaf::Rect(1, 2, 3, 4));
}


TEST(WindowParser, ParseMaxWidthAndHeight) {

    auto xaml = R"(<Window MaxWidth="1" MaxHeight="2"></Window>)";
    auto window = zaf::CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetMaxWidth(), 1);
    ASSERT_EQ(window->GetMaxHeight(), 2);

    xaml = R"(
        <Window>
            <Window.MaxWidth>3</Window.MaxWidth>
            <Window.MaxHeight>4</Window.MaxHeight>
        </Window>
    )";
    window = zaf::CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetMaxWidth(), 3);
    ASSERT_EQ(window->GetMaxHeight(), 4);
}


TEST(WindowParser, ParseMinAndMaxSize) {

    auto xaml = R"(<Window MinSize="10,20" MaxSize="30,40"></Window>)";
    auto window = zaf::CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetMinSize(), zaf::Size(10, 20));
    ASSERT_EQ(window->GetMaxSize(), zaf::Size(30, 40));

    xaml = R"(
        <Window>
            <Window.MinSize Width="50" Height="60"></Window.MinSize>
            <Window.MaxSize Width="70" Height="80"></Window.MaxSize>
        </Window>
    )";
    window = zaf::CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetMinSize(), zaf::Size(50, 60));
    ASSERT_EQ(window->GetMaxSize(), zaf::Size(70, 80));
}


TEST(WindowParser, ParseCanMinimizeAndCanMaximize) {

    auto xaml = R"(<Window CanMinimize="false" CanMaximize="false" />)";
    auto window = zaf::CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->CanMinimize(), false);
    ASSERT_EQ(window->CanMaximize(), false);

    xaml = R"(
        <Window>
            <Window.CanMinimize>true</Window.CanMinimize>
            <Window.CanMaximize>true</Window.CanMaximize>
        </Window>
    )";
    window = zaf::CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->CanMinimize(), true);
    ASSERT_EQ(window->CanMaximize(), true);
}


TEST(WindowParser, ParseIsToolWindow) {

    auto window = zaf::CreateObjectFromXaml<zaf::Window>(R"(<Window IsToolWindow="true" />)");
    ASSERT_EQ(window->IsToolWindow(), true);

    window = zaf::CreateObjectFromXaml<zaf::Window>(R"(
        <Window><Window.IsToolWindow>false</Window.IsToolWindow></Window>
    )");
    ASSERT_EQ(window->IsToolWindow(), false);
}


TEST(WindowParser, ParseIsSizable) {

    auto window = zaf::CreateObjectFromXaml<zaf::Window>(R"(<Window IsSizable="false" />)");
    ASSERT_EQ(window->IsSizable(), false);

    window = zaf::CreateObjectFromXaml<zaf::Window>(R"(
        <Window><Window.IsSizable>true</Window.IsSizable></Window>
    )");
    ASSERT_EQ(window->IsSizable(), true);
}


TEST(WindowParser, ParseHasSystemMenu) {

    auto window = zaf::CreateObjectFromXaml<zaf::Window>(R"(<Window HasSystemMenu="false" />)");
    ASSERT_EQ(window->HasSystemMenu(), false);

    window = zaf::CreateObjectFromXaml<zaf::Window>(R"(
        <Window><Window.HasSystemMenu>true</Window.HasSystemMenu></Window>
    )");
    ASSERT_EQ(window->HasSystemMenu(), true);
}


TEST(WindowParser, ParseRootControl) {

    //Force initialization global objects.
    zaf::Label::Type->GetName();
    zaf::Button::Type->GetName();

    auto window = zaf::CreateObjectFromXaml<zaf::Window>(R"(<Window RootControl="Label" />)");
    auto label = std::dynamic_pointer_cast<zaf::Label>(window->GetRootControl());
    ASSERT_NE(label, nullptr);

    window = zaf::CreateObjectFromXaml<zaf::Window>(R"(
        <Window>
            <Window.RootControl>
                <Button Text="bbbb"></Button>
            </Window.RootControl>
        </Window>
    )");
    auto button = std::dynamic_pointer_cast<zaf::Button>(window->GetRootControl());
    ASSERT_NE(button, nullptr);
    ASSERT_EQ(button->Text(), L"bbbb");
}


TEST(WindowParser, ParseControls) {

    auto window = zaf::CreateObjectFromXaml<zaf::Window>(R"(
        <Window>
            <Control Name="111" />
            <Control Name="222" />
        </Window>
    )");

    const auto& controls = window->GetRootControl()->Children();
    ASSERT_EQ(controls.size(), 2);
    ASSERT_EQ(controls[0]->Name(), L"111");
    ASSERT_EQ(controls[1]->Name(), L"222");
}