#include <memory>
#include <string>

#include "clip.h"
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

using namespace ftxui;

struct bla
{
    std::string label;
    std::string username;
    std::string password;
};

std::vector<bla> passwords;

Component addNewLoginComponent(std::function<void()> do_nothing, std::function<void()> hide_modal)
{
    std::string new_label;
    std::string new_username;
    std::string new_password;

    InputOption password_option;
    password_option.password = true;
    auto component = Container::Vertical(
        {Input(&new_label, "label: "),
         Input(&new_username, "username: "),
         Input(&new_password, "password: ", password_option),
         Button("create", [&]()
                { passwords.push_back({new_label, new_username, new_password}); hide_modal(); }),
         Button("cancel", [&]()
                { hide_modal(); })});
    component |= Renderer(
        [&](Element inner)
        {
            return vbox({text("new login"),
                         separator(),
                         inner});
        });

    return component;
};

Component mainComponent(std::function<void()> show_modal, std::function<void()> exit)
{
    passwords.push_back({"reddit", "asdfasdfasdf", "abskdlfjkdsjdklf"});
    passwords.push_back({"facebook", "asdfasdfasdf", "abskdlfjkdsjdklf"});
    passwords.push_back({"youtube", "asdfasdfasdf", "abskdlfjkdsjdklf"});

    std::vector<Component> menu_items;

    for (const auto &password : passwords)
    {
        menu_items.push_back(MenuEntry(password.label));
    }

    auto status_box = text("press 'u' to copy the username and 'p' to copy the password");

    int selected = 0;
    auto menu = Container::Vertical(menu_items, &selected);
    menu |= CatchEvent(
        [&](Event event)
        {
            if (event.character() == "u")
            {
                status_box = text("copied username for " + passwords[selected].label + " to clipboard");
                clip::set_text(passwords[selected].username);
                return true;
            }
            else if (event.character() == "p")
            {
                status_box = text("copied password for " + passwords[selected].label + " to clipboard");
                clip::set_text(passwords[selected].password);
                return true;
            }
            else if (event.character() == "n")
            {
                show_modal();
            }

            return false;
        });

    menu |= Renderer(
        [&](Element inner)
        { return vbox({
                     text("selected = " + passwords[selected].label) | center,
                     separator(),
                     inner | flex,
                     separator(),
                     status_box | center,
                 }) |
                 border; });
    // auto component = Renderer(
    //     menu,
    //     [&]
    //     { return vbox({
    //                  text("selected = " + passwords[selected].label) | center,
    //                  separator(),
    //                  menu->Render() | flex,
    //                  separator(),
    //                  status_box | center,
    //              }) |
    //              border; });
    return menu;
}

int main()
{
    auto screen = ScreenInteractive::TerminalOutput();

    auto modal_shown = false;

    auto show_modal = [&]
    { modal_shown = true; };
    auto hide_modal = [&]
    { modal_shown = false; };

    auto exit = screen.ExitLoopClosure();
    auto do_nothing = [&] {};
    auto main_component = mainComponent(show_modal, exit);
    auto modal_component = addNewLoginComponent(do_nothing, hide_modal);

    main_component |= Modal(modal_component, &modal_shown);
    screen.Loop(main_component);
    return 0;
}
