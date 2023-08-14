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

struct login_struct
{
    std::string label;
    std::string username;
    std::string password;
};

Component Wrap(std::string name, Component component)
{
    return Renderer(
        component,
        [name, component]
        { return hbox({
                     text(name) | size(WIDTH, EQUAL, 8),
                     separator(),
                     component->Render() | xflex,
                 }) |
                 xflex; });
}

int main()
{
    auto screen = ScreenInteractive::Fullscreen();

    auto status_box = text("Status: ");
    auto shortcut_box = text("Shortcuts: 'u' - copy username, 'p' - copy passwordm, 'd' - delete entry.");

    std::vector<login_struct> passwords;
    passwords.push_back({"reddit", "asdfasdfasdf", "abskdlfjkdsjdklf"});
    passwords.push_back({"facebook", "asdfasdfasdf", "abskdlfjkdsjdklf"});
    passwords.push_back({"youtube", "asdfasdfasdf", "abskdlfjkdsjdklf"});

    std::vector<std::string> menu_items;
    for (const auto &password : passwords)
    {
        menu_items.push_back(password.label);
    }
    auto menu_selected = 0;
    auto menu = Menu(&menu_items, &menu_selected);

    menu |= CatchEvent(
        [&](Event event)
        {
            if (event.character() == "u")
            {
                status_box = text("Status: copied username for " + passwords[menu_selected].label + " to clipboard");
                clip::set_text(passwords[menu_selected].username);
                return true;
            }
            else if (event.character() == "p")
            {
                status_box = text("Status: copied password for " + passwords[menu_selected].label + " to clipboard");
                clip::set_text(passwords[menu_selected].password);
                return true;
            }
            else if (event.character() == "d")
            {
                status_box = text("Status: deleted entry: " + passwords[menu_selected].label);
                menu_items.erase(menu_items.begin() + menu_selected);
                passwords.erase(passwords.begin() + menu_selected);
            }

            return false;
        });
    menu = Wrap("Logins", menu);

    std::string new_label, new_username, new_password;
    auto label_input = Input(&new_label, "label");
    label_input = Wrap("New Label", label_input);

    auto username_input = Input(&new_username, "username");
    username_input = Wrap("New Username", username_input);

    auto password_input = Input(&new_password, "password");
    password_input = Wrap("New Password", password_input);

    std::string button_label = "Add";
    std::function<void()> on_button_clicked_;
    auto add_button = Button(
        &button_label,
        [&]()
        {
            if (new_label.empty() || new_username.empty())
                return;
            passwords.push_back({new_label, new_username, new_password});
            menu_items.push_back(new_password);
        });
    add_button = Wrap("Add new password", add_button);

    auto layout = Container::Vertical(
        {menu,
         label_input,
         username_input,
         password_input,
         add_button});

    auto component = Renderer(layout, [&]
                              { return vbox(
                                           {menu->Render() | flex,
                                            separator(),
                                            label_input->Render(),
                                            username_input->Render(),
                                            password_input->Render(),
                                            add_button->Render(),
                                            separator(),
                                            status_box,
                                            separator(),
                                            shortcut_box}) |
                                       xflex | border; });
    screen.Loop(component);
    return 0;
}
